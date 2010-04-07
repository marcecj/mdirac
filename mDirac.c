#include <mex.h>
#include <Dirac.h>
#include <math.h>
#include <string.h>

typedef struct {
    double*    dirac_input_data;
    float      dirac_ts_factor;
    int        dirac_mode;
    int        dirac_cur_instance;
    float      fs;
    int        input_num_channels;
    int        input_num_samples;
    int        cur_sample;
} DIRAC_STATE;

DIRAC_STATE dirac_state;
void** dirac;
    
void initiate_state(DIRAC_STATE *state, int nrhs, const mxArray *prhs[])
{
    /* initiate some variables */
    state->cur_sample = 0;

    if( nrhs < 1 )
        mexErrMsgTxt("Missing argument: you need to at least pass an array or a file name.");
    
    if( mxIsEmpty(prhs[0]) )
        mexErrMsgTxt("Please no empty arrays!");
    else if( mxIsNumeric(prhs[0]) ) {
        state->dirac_input_data   = mxGetPr(prhs[0]);
        state->input_num_channels = mxGetM(prhs[0]);
        state->input_num_samples  = mxGetN(prhs[0]);
    }
    else
        mexErrMsgTxt("Only double array are accepted as first argument.");

    if( nrhs < 2 )
        state->fs = 44100.f;
    else
    {
        state->fs = (float)*mxGetPr(prhs[1]);
        mexPrintf("fs = %f\n", state->fs);
        if( state->fs != 44100 && state->fs != 48000 ) {
            mexWarnMsgTxt("Sampling rate can either be 44100 or 4800 Hz. Defaulting to 44100 Hz.");
            state->fs = 44100.f;
        }
    }

    if( nrhs < 3 )
        state->dirac_ts_factor = 1.15f;
    else
    {
        state->dirac_ts_factor = (float)*mxGetPr(prhs[2]);
        if( state->dirac_ts_factor < 0.5 ) {
            mexWarnMsgTxt("Time stretch factor out of bounds, limiting to range [0.5;2.0].");
            state->dirac_ts_factor = 0.5;
        }
        else if( state->dirac_ts_factor > 2.0 ) {
            mexWarnMsgTxt("Time stretch factor out of bounds, limiting to range [0.5;2.0].");
            state->dirac_ts_factor = 2.0;
        }
    }

    if( nrhs < 4 )
        state->dirac_mode = kDiracLambdaPreview; 
    else
    {
        state->dirac_mode = (int)mxGetScalar(prhs[3]);
        if( state->dirac_mode < 0 || state->dirac_mode > 5 ) {
            mexWarnMsgTxt("Dirac mode out of bounds: defaulting to Preview.");
            state->dirac_mode = kDiracLambdaPreview;
        }
    }
}

long fill_buffer(float* data, long num_frames, void* dirac_state)
{
    DIRAC_STATE* state = (DIRAC_STATE*)dirac_state;

    long N=0;
    int i;

    if( state->cur_sample + num_frames < state->input_num_samples ) {
        N = num_frames;
    } else if( state->cur_sample < state->input_num_samples ) {
        N = state->input_num_samples - state->cur_sample;
    } else {
        N = 0;
        state->cur_sample = 0;
    }

    /*
     * Copy input data to Diracs input buffer.
     */

    for( i=0; i<N; i++ )
        data[i] = (float)state->dirac_input_data[
            state->dirac_cur_instance +
            state->input_num_channels*state->cur_sample++];

    return N;
}

/* function to clear memory at exit */
void clear_memory(void)
{
    int i;
    for( i=0; i<dirac_state.input_num_channels; i++ )
        if( !dirac[i] )
            DiracDestroy(dirac[i]);
    if( !dirac )
        free(dirac);
}

void mexFunction(int nlhs, mxArray *plhs[],
                 int nrhs, const mxArray *prhs[])
{
    /* various initiations */

    int i, j; // counter in for-loops
    long new_size, num_frames, ret, total_size;
    double* output;
    float* output_data;
    mexAtExit(&clear_memory);

    initiate_state(&dirac_state, nrhs, prhs);

    /* initiate one Dirac instance per channel */

    dirac = (void**)mxMalloc(dirac_state.input_num_channels*sizeof(void*));
    if( !dirac )
        mexErrMsgTxt("Could not create Dirac object array, aborting.\n");

    /* Create one dirac instance per channel beforehand, instead of constantly
     * creating/destroying in the second for-loop. */
    for( i=0; i<dirac_state.input_num_channels; i++ )
    {
        dirac[i] = DiracCreateInterleaved(kDiracLambdaPreview,
                kDiracQualityPreview, 1, dirac_state.fs,
                &fill_buffer);
        if( !dirac[i] )
            mexErrMsgTxt("Could not create Dirac object, aborting.\n");

        /* set the stretch/shift factors */
        DiracSetProperty(kDiracPropertyTimeFactor, dirac_state.dirac_ts_factor, dirac[i]);
        DiracSetProperty(kDiracPropertyPitchFactor, 1.0, dirac[i]);
        DiracSetProperty(kDiracPropertyFormantFactor, 1.0, dirac[i]);
    }

    num_frames    = 8192;
    new_size = (int)ceilf(dirac_state.dirac_ts_factor * dirac_state.input_num_samples);
    plhs[0]            = mxCreateDoubleMatrix((mwSize)dirac_state.input_num_channels, (mwSize)new_size, mxREAL);
    output     = mxGetPr(plhs[0]);
    mexPrintf("Output will be %ix%i Matrix\n", mxGetM(plhs[0]), mxGetN(plhs[0]));

    output_data = (float*)mxCalloc(num_frames, sizeof(float));
    if( output_data == NULL )
        mexErrMsgTxt("mxCalloc output_data failed!");

    for( i=0; i<dirac_state.input_num_channels; i++ )
    {
        dirac_state.dirac_cur_instance = i;
        ret=0;
        total_size=0;

        do {
            int correction_factor = 0;

            /* mexPrintf("Before DiracProcess..."); */
            ret = DiracProcessInterleaved(output_data, num_frames,
                    (void*)&dirac_state, dirac[i]);
            /* mexPrintf("ret = %i\n", ret); */

            total_size += ret;
            if( total_size > new_size )
                correction_factor = total_size - new_size;
            correction_factor = correction_factor > 0 ? correction_factor : 0;

            /* mexPrintf("range [%i;%i]\n", i + total_size-ret, i + total_size-ret + (ret-correction_factor)*dirac_state.input_num_channels); */
            for( j=0; j<ret-correction_factor; j++)
                output[i + (total_size-ret + j)*dirac_state.input_num_channels] = (double)output_data[j];
        }
        while( ret > 0 );
    }

    mexPrintf("Done...\n");
}
