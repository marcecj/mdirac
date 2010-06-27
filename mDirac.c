#include <math.h>
#include <mex.h>
#include <Dirac.h>

/* struct type containing various state variables */
typedef struct {
    double*    dirac_input_data;
    float      dirac_ts_factor;     // time stretch factor
    int        dirac_mode;          // mode (lambda parameter)
    int        dirac_qual;          // quality
    int        dirac_cur_channel;  // one instance per channel
    float      fs;
    int        input_num_channels;
    int        input_num_samples;
    int        cur_sample;          // current sample
} DIRAC_STATE;

/* global variables, so that clear_memory() can reach them */
void* dirac;
    
void initialise_state(DIRAC_STATE *state, int nrhs, const mxArray *prhs[])
{
    /* initialise some variables */
    state->cur_sample = 0;

    if( nrhs < 1 )
        mexErrMsgTxt("Missing argument: you need to at least pass an array or a file name.");
    
    if( mxIsEmpty(prhs[0]) )
        mexErrMsgTxt("Please no empty arrays!");
    else if( mxIsNumeric(prhs[0]) ) {
        state->dirac_input_data   = mxGetPr(prhs[0]);
        state->input_num_samples  = mxGetM(prhs[0]);
        state->input_num_channels = mxGetN(prhs[0]);
    }
    else
        mexErrMsgTxt("Only double arrays are accepted as first argument.");

    if( nrhs < 2 )
        state->fs = 44100.f;
    else if( mxIsNumeric(prhs[1]) ) {
        state->fs = (float)*mxGetPr(prhs[1]);
        if( state->fs != 44100 && state->fs != 48000 )
            mexErrMsgTxt("Sampling rate must either be 44.1 or 48 kHz, you need to re-sample your data beforehand.");
    }
    else
        mexErrMsgTxt("fs must be a numeric data type.");

    if( nrhs < 3 || mxIsEmpty(prhs[2]) )
        state->dirac_ts_factor = 1.f;
    else if( mxIsNumeric(prhs[2]) )
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
    else
        mexErrMsgTxt("Time stretch factor must be a numeric data type.");

    if( nrhs < 4 || mxIsEmpty(prhs[3]) )
        state->dirac_qual = kDiracLambdaPreview; 
    else if( mxIsNumeric(prhs[3]) )
    {
        switch( (int)mxGetScalar(prhs[3]) ) {
            case 0: state->dirac_mode = kDiracLambdaPreview; break;
            case 1: state->dirac_mode = kDiracLambda1; break;
            case 2: state->dirac_mode = kDiracLambda2; break;
            case 3: state->dirac_mode = kDiracLambda3; break;
            case 4: state->dirac_mode = kDiracLambda4; break;
            case 5: state->dirac_mode = kDiracLambda5; break;
            default:
                    mexWarnMsgTxt("Bad Dirac mode: defaulting to mode 3.");
                    state->dirac_mode = kDiracLambda3;
        }
    }
    else
        mexErrMsgTxt("Mode setting must be a numeric data type.");

    if( nrhs < 5 || mxIsEmpty(prhs[4]) )
        state->dirac_qual = kDiracQualityPreview; 
    else if( mxIsNumeric(prhs[4]) )
    {
        switch( (int)mxGetScalar(prhs[4]) ) {
            case 0: state->dirac_qual = kDiracQualityPreview; break;
            case 1: state->dirac_qual = kDiracQualityGood; break;
            case 2: state->dirac_qual = kDiracQualityBetter; break;
            case 3: state->dirac_qual = kDiracQualityBest; break;
            default:
                    mexWarnMsgTxt("Bad Dirac quality: defaulting to medium quality.");
                    state->dirac_qual = kDiracQualityGood;
        }
    }
    else
        mexErrMsgTxt("Quality setting must be a numeric data type.");
}

long fill_buffer(float* data, long num_frames, void* dirac_state)
{
    DIRAC_STATE* state = (DIRAC_STATE*)dirac_state;

    long N=0;
    int i;

    if( state->cur_sample + num_frames < state->input_num_samples )
        N = num_frames;
    else if( state->cur_sample < state->input_num_samples )
        N = state->input_num_samples - state->cur_sample;
    else {
        N = 0;
        state->cur_sample = 0;
    }

    /* Copy input data to Diracs input buffer. */
    for( i=0; i<N; i++ )
        data[i] = (float)state->dirac_input_data[
            state->dirac_cur_channel*state->input_num_samples +
            state->cur_sample++];

    return N;
}

/* function to clear memory at exit */
void clear_memory(void)
{
    if( dirac != NULL )
        DiracDestroy(dirac);
}

void mexFunction(int nlhs, mxArray *plhs[],
                 int nrhs, const mxArray *prhs[])
{
    /* various initiations */

    DIRAC_STATE dirac_state;
    int         i, j; //counters in for-loops
    long        new_size;
    const long  num_frames=8192;
    double*     output;
    float*      dirac_output;

    mexAtExit(&clear_memory);
    initialise_state(&dirac_state, nrhs, prhs);

    dirac = (void*)mxMalloc(dirac_state.input_num_channels);
    if( dirac == NULL )
        mexErrMsgTxt("Could not create Dirac object array, aborting.\n");

    new_size = (int)ceilf(dirac_state.dirac_ts_factor * dirac_state.input_num_samples);
    plhs[0]  = mxCreateDoubleMatrix((mwSize)new_size, (mwSize)dirac_state.input_num_channels, mxREAL);
    output   = mxGetPr(plhs[0]);

    dirac_output = (float*)mxCalloc(num_frames, sizeof(float));
    if( dirac_output == NULL )
        mexErrMsgTxt("mxCalloc of dirac_output failed!");

    /* start processing */
    for( i=0; i<dirac_state.input_num_channels; i++ )
    {
        long ret=0, total_size=0;
        dirac_state.dirac_cur_channel = i;

        /* create a new Dirac instance every channel, to prevent the necessity
         * of an arbitrary limit on the number of channels; the overhead
         * doesn't seem to be too large for normal audio signals */
        dirac = DiracCreateInterleaved(dirac_state.dirac_mode,
                dirac_state.dirac_qual, 1, dirac_state.fs,
                &fill_buffer);
        if( dirac == NULL )
            mexErrMsgTxt("Could not create Dirac object, aborting.\n");

        /* set the stretch/shift factors */
        DiracSetProperty(kDiracPropertyTimeFactor, dirac_state.dirac_ts_factor, dirac);
        DiracSetProperty(kDiracPropertyPitchFactor, 1.0, dirac);
        DiracSetProperty(kDiracPropertyFormantFactor, 1.0, dirac);

        do {
            int correction_factor=0;

            ret = DiracProcessInterleaved(dirac_output, num_frames,
                    (void*)&dirac_state, dirac);

            total_size += ret;
            /* correction factor, so that we don't attempt to read more than
             * new_size samples */
            if( total_size > new_size )
                correction_factor = total_size - new_size;
            if( correction_factor < 0 )
                correction_factor = 0;

            for( j=0; j<ret-correction_factor; j++)
                output[i*new_size + total_size-ret+j] = (double)dirac_output[j];
        }
        while( ret > 0 );

        DiracDestroy(dirac);
        /* Set to NULL, otherwise dirac could be destroyed twice */
        dirac = NULL; 
    }
}
