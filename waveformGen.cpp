
//TX2 Design
// 3/1/2016
// Written by: Tien Le

//Waveform mapping using BPSK Modulation
//Take a carrier sinusoid and modulate its phase depending on whether the input bit is a 1 or a 0

#include <stdio.h>
#include <math.h>
#include "portaudio.h"
#include "waveformGen.h"
#include <cassert>

    BPSK_modulator::BPSK_modulator() : stream(0), phase(0),prev_input(0),counter(0), BPSK_phase(false) 
    {
	assert(TABLE_SIZE%2 == 0);
        /* initialise sinusoidal wavetable for 0 and 1 */
        for( int i=0; i<TABLE_SIZE; i++ )
        {
            sine1[i] = (float)(AMPLITUDE * sin( ((double)i/(double)TABLE_SIZE) * M_PI * 2. ));
            sine0[i] = (float)(AMPLITUDE * sin( ((double)( (i+(TABLE_SIZE/2)) % TABLE_SIZE )/(double)TABLE_SIZE) * M_PI * 2. ));
        }

        sprintf( message, "No Message" );
    }

    bool BPSK_modulator::open(PaDeviceIndex index)
    {
        PaStreamParameters outputParameters;

        outputParameters.device = index;
        if (outputParameters.device == paNoDevice) {
            return false;
        }

        outputParameters.channelCount = 1;       /* mono output */
        outputParameters.sampleFormat = paFloat32; /* 32 bit floating point output */
        outputParameters.suggestedLatency = Pa_GetDeviceInfo( outputParameters.device )->defaultLowOutputLatency;
        outputParameters.hostApiSpecificStreamInfo = NULL;

        PaError err = Pa_OpenStream(
            &stream,
            NULL, /* no input */
            &outputParameters,
            SAMPLE_RATE,
            FRAMES_PER_BUFFER,
            paClipOff,      /* we won't output out of range samples so don't bother clipping them */
            &BPSK_modulator::paCallback,
            this            /* Using 'this' for userData so we can cast to BPSK_Modulator* in paCallback method */
            );

        if (err != paNoError)
        {
            /* Failed to open stream to device! */
            return false;
        }

        err = Pa_SetStreamFinishedCallback( stream, &BPSK_modulator::paStreamFinished );

        if (err != paNoError)
        {
            Pa_CloseStream( stream );
            stream = 0;

            return false;
        }

        return true;
    }

    bool BPSK_modulator::close()
    {
        if (stream == 0)
            return false;

        PaError err = Pa_CloseStream( stream );
        stream = 0;

        return (err == paNoError);
    }


    bool BPSK_modulator::start()
    {
        if (stream == 0)
            return false;

        PaError err = Pa_StartStream( stream );

        return (err == paNoError);
    }

    bool BPSK_modulator::stop()
    {
        if (stream == 0)
            return false;

        PaError err = Pa_StopStream( stream );

        return (err == paNoError);
    }


    /* The instance callback, where we have access to every method/variable in object of the class */
    int BPSK_modulator::paCallbackMethod(const void *inputBuffer, void *outputBuffer,
        unsigned long framesPerBuffer,
        const PaStreamCallbackTimeInfo* timeInfo,
        PaStreamCallbackFlags statusFlags)
    {
        float *out = (float*)outputBuffer;

        (void) timeInfo; /* Prevent unused variable warnings. */
        (void) statusFlags;
        (void) inputBuffer;

	symbolgen(out, inputBuffer, framesPerBuffer);
        return paContinue;

    }


    /* This routine will be called by the PortAudio engine when audio is needed.
    ** It may called at interrupt level on some machines so don't do anything
    ** that could mess up the system like calling malloc() or free().
    */
    int BPSK_modulator::paCallback( const void *inputBuffer, void *outputBuffer,
        unsigned long framesPerBuffer,
        const PaStreamCallbackTimeInfo* timeInfo,
        PaStreamCallbackFlags statusFlags,
        void *userData )
    {
        /* Here we cast userData to BPSK_modulator* type so we can call the instance method paCallbackMethod, we can do that since 
           we called Pa_OpenStream with 'this' for userData */
        return ((BPSK_modulator*)userData)->paCallbackMethod(inputBuffer, outputBuffer,
            framesPerBuffer,
            timeInfo,
            statusFlags);
    }

    void BPSK_modulator::paStreamFinishedMethod()
    {
        printf( "Stream Completed: %s\n", message );
    }

    /*
     * This routine is called by portaudio when playback is done.
     */
    void BPSK_modulator::paStreamFinished(void* userData)
    {
        return ((BPSK_modulator*)userData)->paStreamFinishedMethod();
    }


//This function is used to generate the output buffer for the paCallbackMethod function
//We will generate the symbols for BPSK in here based on the input buffer
//We will keep track of a "state" variable
    void BPSK_modulator::symbolgen(float* out, const void* inputbuffer,
		unsigned long framesPerBuffer)
    {
        unsigned long i;
	//Test code

	int testInput[framesPerBuffer];
	for(unsigned long j = 0; j < framesPerBuffer; j++){
		if(j%4 == 0)
			testInput[j] = 0;
		else
			0testInput[j] = 1;
		//printf("%d\n", testInput[j]);
	}
 
        for( i=0; i<framesPerBuffer; i++ )
        {
	    printf("Output: %f, Input Bit: %d, Phase: %d\n", *(out-1), BPSK_phase, phase);
	    //output buffer writes to mono output

	    if(BPSK_phase == 1)
	            *out++ = sine1[phase];
	    else
		    *out++ = sine0[phase];  

            phase += 1;

            if( phase >= TABLE_SIZE ){ 
		//Reached the end of the current symbol
		//Move the current bit to the next symbol		
		prev_input = testInput[counter];
		counter = (counter + 1)%framesPerBuffer;

		//Reset the sinusoid
		phase -= TABLE_SIZE;
		//BPSK: Shift the phase by 180 degrees if the input signal changes
		if( (prev_input^testInput[counter]) == 1){
			//Set the sinusoid to the BPSK_Phase
			BPSK_phase = testInput[counter];
			printf("Output flipped, the Phase is now: %d \n", BPSK_phase);
		}
		};

        }
    }







