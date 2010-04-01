#include <mex.h>
#include <Dirac.h>
#include <sndfile.h>
#include <math.h>
#include <string.h>

/* function OutData = TimeStretchDirac(InData,fs,TimeStretchFaktor, Mode) */
typedef struct {
    SF_INFO* file_info;
    SNDFILE* input_file;
    char* input_file_name;
    double* input_data;
    float fs;
    float ts_factor;
    int dirac_mode;
    int num_channels;
    int num_samples;
    int cur_sample;
    bool data_is_from_file;
} DIRAC_STATE;
    
void initiate_state(DIRAC_STATE *state, int nrhs, const mxArray *prhs[])
{
    /* initiate some variables */
    state->cur_sample = 0;
    state->file_info->format = 0;

    if( nrhs < 1 )
        mexErrMsgTxt("Missing argument: you need to at least pass an array or a file name.");
    mexPrintf("nrhs >= 1\n");
    
    if( mxIsEmpty(prhs[0]) )
        mexErrMsgTxt("Please no empty arrays!");
    else if( mxIsChar(prhs[0]) ) {
        /* FIXME: something is going wrong here (I think) */
        mxGetString(prhs[0], state->input_file_name, mxGetM(prhs[0])+1);
        state->data_is_from_file = true;
    }
    else if( mxIsNumeric(prhs[0]) ) {
        state->data_is_from_file = false;
        state->input_data = mxGetPr(prhs[0]);
        state->num_channels  = mxGetM(prhs[0]);
        state->num_samples   = mxGetN(prhs[0]);
        mexPrintf("num_samples = %i\n", state->num_samples);
    }
    else
        mexErrMsgTxt("Only double array and strings are accepted as first argument.");

    if( nrhs < 2 )
        state->fs = 44100.f;
    else
    {
        state->fs = *mxGetPr(prhs[1]);
        if( state->fs != 44100 && state->fs != 48000 ) {
            mexWarnMsgTxt("Sampling rate can either be 44100 or 4800 Hz. Defaulting to 44100 Hz.");
            state->fs = 44100.f;
        }
    }

    if( nrhs < 3 )
        state->ts_factor = 1.15;
    else
    {
        state->ts_factor = *mxGetPr(prhs[2]);
        if( state->ts_factor < 0.5 || state->ts_factor > 2.0 ) {
            mexWarnMsgTxt("Time stretch factor out of bounds, limiting to range [0.5;2.0].");
            state->ts_factor = fmin(0.5, fmax(state->ts_factor, 2.0));
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

    long N=0, processed_frames=0;
    if( state->data_is_from_file )
    {
        /* TODO: this needs to be completed */
        processed_frames = sf_read_float(state->input_file, data, num_frames);
    }
    else
    {
        if( state->cur_sample + num_frames < state->num_samples ) {
            N = num_frames;
        } else if( state->cur_sample < state->num_samples ) {
            N = state->num_samples - state->cur_sample;
        } else {
            N = 0;
        }

        /*
         * Copy input data to Diracs input buffer.
         */

         /*
         * TODO: this should work, but the resulting array is full of NaNs
         *
          * float* temp_in = (float*)state->input_data;
          * memmove(data, temp_in + state->cur_sample, N*sizeof(float));
          * state->cur_sample += N;
          */
         
        int i;
        for( i=0; i<N; i++ )
            data[i] = (float)state->input_data[state->cur_sample++];

        processed_frames = N;
    }

    return processed_frames;
}

void mexFunction(int nlhs, mxArray *plhs[],
                 int nrhs, const mxArray *prhs[])
{
    DIRAC_STATE dirac_state;
    SF_INFO file_info;
    dirac_state.file_info = &file_info;

    initiate_state(&dirac_state, nrhs, prhs);

    void *dirac = DiracCreateInterleaved(kDiracLambdaPreview,
            kDiracQualityPreview, dirac_state.num_channels, dirac_state.fs,
            &fill_buffer);

    if( !dirac )
        mexErrMsgTxt("Could not create Dirac object, aborting.\n");

    /* set the stretch/shift factors */
    DiracSetProperty(kDiracPropertyTimeFactor, dirac_state.ts_factor, dirac);
    DiracSetProperty(kDiracPropertyPitchFactor, 1.0, dirac);
    DiracSetProperty(kDiracPropertyFormantFactor, 1.0, dirac);

    if( dirac_state.data_is_from_file )
        dirac_state.input_file = sf_open(dirac_state.input_file_name, SFM_READ,
                dirac_state.file_info);

    long num_frames    = 8192;
    long new_size = (int)(dirac_state.ts_factor * dirac_state.num_samples + 0.5);
    plhs[0]            = mxCreateDoubleMatrix(1, new_size, mxREAL);
    double* output     = mxGetPr(plhs[0]);
    mexPrintf("%ix%i Matrix\n", mxGetM(plhs[0]), mxGetN(plhs[0]));

    float* output_data = calloc(num_frames, sizeof(float));
    if( output_data == NULL )
        mexErrMsgTxt("calloc output_data failed!");

    long total_size = 0;

    long ret=0;
    do {
        int correction_factor = 0;

        ret = DiracProcessInterleaved(output_data, num_frames,
                (void*)&dirac_state, dirac);
        /* mexPrintf("ret = %i\n", ret); */

        total_size += ret;
        if( total_size > new_size )
            correction_factor = total_size - new_size;

        int i=0;
        for( i; i<ret-correction_factor; i++)
            output[i+total_size-ret] = (double)output_data[i];
    }
    while( ret > 0 );

    mexPrintf("We're through!\n");

    /* free some memory */
    free(output_data);
    DiracDestroy(dirac);
}
