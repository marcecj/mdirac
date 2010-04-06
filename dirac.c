#include <mex.h>
#include <Dirac.h>
#include <sndfile.h>
#include <math.h>
#include <string.h>

/* function OutData = TimeStretchDirac(InData,fs,TimeStretchFaktor, Mode) */
typedef struct {
    SF_INFO*   sf_file_info;
    SNDFILE*   sf_input_file;
    char*      sf_input_fname;
    double*    dirac_input_data;
    float      dirac_ts_factor;
    int        dirac_mode;
    int        dirac_cur_instance;
    float      fs;
    int        input_num_channels;
    int        input_num_samples;
    bool       input_is_from_file;
    int        cur_sample;
} DIRAC_STATE;

DIRAC_STATE dirac_state;
void** dirac;
    
void initiate_state(DIRAC_STATE *state, int nrhs, const mxArray *prhs[])
{
    /* initiate some variables */
    state->cur_sample = 0;
    state->input_is_from_file = false;

    if( nrhs < 1 )
        mexErrMsgTxt("Missing argument: you need to at least pass an array or a file name.");
    /* mexPrintf("nrhs >= 1\n"); */
    
    if( mxIsEmpty(prhs[0]) )
        mexErrMsgTxt("Please no empty arrays!");
    else if( mxIsChar(prhs[0]) )
    {
        int str_size = mxGetN(prhs[0])+1;
        state->sf_input_fname = (char*)mxCalloc(str_size, sizeof(char));
        if( state->sf_input_fname == NULL )
            mexErrMsgTxt("mxCalloc error!");
        mxGetString(prhs[0], state->sf_input_fname, str_size);

        /*
         * initiate sf_file_info struct pointer
         */
        state->sf_file_info = (SF_INFO*)mxMalloc(sizeof(SF_INFO));
        if( !state->sf_file_info )
            mexErrMsgTxt("could not allocate SF_INFO* instance");
        /* format needs to be set to 0 before a file is opened */
        state->sf_file_info->format = 0;

        state->input_is_from_file = true;
        mexPrintf("Opening file...\n");
        state->sf_input_file = sf_open(state->sf_input_fname, SFM_READ,
                state->sf_file_info);
        if( !state->sf_input_file )
            mexErrMsgTxt("could not open sound file");
    }
    else if( mxIsNumeric(prhs[0]) ) {
        state->dirac_input_data   = mxGetPr(prhs[0]);
        state->input_num_channels = mxGetM(prhs[0]);
        state->input_num_samples  = mxGetN(prhs[0]);
        mexPrintf("input_num_samples = %i\n", state->input_num_samples);
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
        state->dirac_ts_factor = 1.15;
    else
    {
        state->dirac_ts_factor = *mxGetPr(prhs[2]);
        if( state->dirac_ts_factor < 0.5 || state->dirac_ts_factor > 2.0 ) {
            mexWarnMsgTxt("Time stretch factor out of bounds, limiting to range [0.5;2.0].");
            state->dirac_ts_factor = fmin(0.5, fmax(state->dirac_ts_factor, 2.0));
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
    if( state->input_is_from_file )
    {
        /* TODO: this needs to be completed */

        /*
         * FIXME: This causes matlab to crash:
         * processed_frames = sf_readf_float(state->sf_input_file, data, num_frames);
         */
        processed_frames = sf_read_float(state->sf_input_file, data, num_frames);

        if( processed_frames > 0 && state->sf_file_info->seekable )
            processed_frames = sf_seek(state->sf_input_file, num_frames, SEEK_CUR);

        int sndfile_error = sf_error(state->sf_input_file);
        if( sndfile_error != SF_ERR_NO_ERROR ) {
            mexWarnMsgTxt("libsndfile error!");
            mexErrMsgTxt(sf_error_number(sndfile_error));
            /* mexWarnMsgTxt(sf_error_number(sndfile_error)); */
        }
    }
    else
    {
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

         /*
         * TODO: this should work, but the resulting array is full of NaNs
         *
          * float* temp_in = (float*)state->dirac_input_data;
          * memmove(data, temp_in + state->cur_sample, N*sizeof(float));
          * state->cur_sample += N;
          */
         
        int i;
        for( i=0; i<N; i++ )
            data[i] = (float)state->dirac_input_data[
                state->dirac_cur_instance*state->input_num_samples +
                state->cur_sample++];

        processed_frames = N;
    }

    return processed_frames;
}

/* function to clear memory at exit */
void clear_memory(void)
{
    if( !dirac_state.sf_input_file )
        sf_close(dirac_state.sf_input_file);
    /* if( !dirac_state.sf_file_info ) */
    /*     free(dirac_state.sf_file_info); */
    if( !dirac )
        DiracDestroy(dirac);
}

void mexFunction(int nlhs, mxArray *plhs[],
                 int nrhs, const mxArray *prhs[])
{
    /*
     * various initiations
     */

    int i; // counter in for-loops
    mexAtExit(&clear_memory);

    initiate_state(&dirac_state, nrhs, prhs);

    if( dirac_state.input_is_from_file ) {
        mexPrintf("Frames\t: %i\n"     , dirac_state.sf_file_info->frames);
        mexPrintf("Samplerate\t: %i\n" , dirac_state.sf_file_info->samplerate);
        mexPrintf("Channels\t: %i\n"   , dirac_state.sf_file_info->channels);
        mexPrintf("Format\t: %i\n"     , dirac_state.sf_file_info->format);
        mexPrintf("Sections\t: %i\n"   , dirac_state.sf_file_info->sections);
        mexPrintf("Seekable\t: %i\n"   , dirac_state.sf_file_info->seekable);
        /* dirac_state.input_num_channels  = dirac_state.sf_file_info->channels; */
        dirac_state.input_num_channels  = 1;
        dirac_state.input_num_samples   = dirac_state.sf_file_info->frames;
    }


    /* 
     * initiate one Dirac instance per channel
     */

    dirac = (void**)mxMalloc(dirac_state.input_num_channels*sizeof(void*));
    if( !dirac )
        mexErrMsgTxt("Could not create Dirac object array, aborting.\n");

    for( i=0; i<dirac_state.input_num_channels; i++ )
    {
        mexPrintf("i = %i\n", i);
        dirac[i] = DiracCreateInterleaved(kDiracLambdaPreview,
                kDiracQualityPreview, 1, dirac_state.fs,
                &fill_buffer);
        if( !dirac[i] )
            mexErrMsgTxt("Could not create Dirac object, aborting.\n");
    }

    /* set the stretch/shift factors */

    for( i=0; i<dirac_state.input_num_channels; i++ )
    {
        DiracSetProperty(kDiracPropertyTimeFactor, dirac_state.dirac_ts_factor, dirac[i]);
        DiracSetProperty(kDiracPropertyPitchFactor, 1.0, dirac[i]);
        DiracSetProperty(kDiracPropertyFormantFactor, 1.0, dirac[i]);
    }

    long num_frames    = 8192;
    long new_size = (int)ceilf(dirac_state.dirac_ts_factor * dirac_state.input_num_samples);
    plhs[0]            = mxCreateDoubleMatrix(dirac_state.input_num_channels, new_size, mxREAL);
    double* output     = mxGetPr(plhs[0]);
    mexPrintf("%ix%i Matrix\n", mxGetM(plhs[0]), mxGetN(plhs[0]));

    float** output_data = mxCalloc(dirac_state.input_num_channels, sizeof(float*));
    if( output_data == NULL )
        mexErrMsgTxt("mxCalloc output_data array failed!");

    for( i=0; i<dirac_state.input_num_channels; i++ )
    {
        output_data[i] = mxCalloc(num_frames, sizeof(float));
        if( output_data[i] == NULL )
            mexErrMsgTxt("mxCalloc output_data failed!");
    }

    for( i=0; i<dirac_state.input_num_channels; i++ )
    {
        dirac_state.dirac_cur_instance = i;
        long ret=0, total_size=0;
        do {
            int correction_factor = 0;

            mexPrintf("Before DiracProcess...");
            ret = DiracProcessInterleaved(output_data[i], num_frames,
                    (void*)&dirac_state, dirac[i]);
            mexPrintf("ret = %i\n", ret);

            total_size += ret;
            if( total_size > new_size )
                correction_factor = total_size - new_size;

            int j;
            for( j=0; j<ret-correction_factor; j++)
                output[i*new_size + j + total_size - ret] = (double)output_data[i][j];
        }
        while( ret > 0 );
    }

    mexPrintf("We're through!\n");

    /*
     * free some memory
     * sf_close(dirac_state.sf_input_file);
     * DiracDestroy(dirac);
     */
}
