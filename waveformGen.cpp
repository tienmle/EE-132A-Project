
//TX2 Design
// 3/1/2016
// Written by: Tien Le

//Waveform mapping using FSK Modulation
//Take a carrier sinusoid and modulate its phase depending on whether the input bit is a 1 or a 0

#include <stdio.h>
#include <math.h>
#include "portaudio.h"
#include "waveformGen.h"
#include <cassert>

    FSK_modulator::FSK_modulator() : stream(0), phase(0), counter(0), FSK_phase(false) 
    {
	assert(TABLE_SIZE%2 == 0);
        /* initialise sinusoidal wavetable for 0 and 1 */
        for( int i=0; i<TABLE_SIZE; i++ )
        {
            sine1[i] = (float)(AMPLITUDE * sin( ((double)i/(double)TABLE_SIZE) * M_PI * 2.* 1.5 ));
            sine0[i] = (float)(AMPLITUDE * sin( ((double)i/(double)TABLE_SIZE) * M_PI * 2. ));
        }

        sprintf( message, "No Message" );
    }

    bool FSK_modulator::open(PaDeviceIndex index)
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
            &FSK_modulator::paCallback,
            this            /* Using 'this' for userData so we can cast to FSK_Modulator* in paCallback method */
            );

        if (err != paNoError)
        {
            /* Failed to open stream to device! */
            return false;
        }

        err = Pa_SetStreamFinishedCallback( stream, &FSK_modulator::paStreamFinished );

        if (err != paNoError)
        {
            Pa_CloseStream( stream );
            stream = 0;

            return false;
        }

        return true;
    }

    bool FSK_modulator::close()
    {
        if (stream == 0)
            return false;

        PaError err = Pa_CloseStream( stream );
        stream = 0;

        return (err == paNoError);
    }


    bool FSK_modulator::start()
    {
        if (stream == 0)
            return false;

        PaError err = Pa_StartStream( stream );

        return (err == paNoError);
    }

    bool FSK_modulator::stop()
    {
        if (stream == 0)
            return false;

        PaError err = Pa_StopStream( stream );

        return (err == paNoError);
    }


    /* The instance callback, where we have access to every method/variable in object of the class */
    int FSK_modulator::paCallbackMethod(const void *inputBuffer, void *outputBuffer,
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
    int FSK_modulator::paCallback( const void *inputBuffer, void *outputBuffer,
        unsigned long framesPerBuffer,
        const PaStreamCallbackTimeInfo* timeInfo,
        PaStreamCallbackFlags statusFlags,
        void *userData )
    {
        /* Here we cast userData to FSK_modulator* type so we can call the instance method paCallbackMethod, we can do that since 
           we called Pa_OpenStream with 'this' for userData */
        return ((FSK_modulator*)userData)->paCallbackMethod(inputBuffer, outputBuffer,
            framesPerBuffer,
            timeInfo,
            statusFlags);
    }

    void FSK_modulator::paStreamFinishedMethod()
    {
        printf( "Stream Completed: %s\n", message );
    }

    /*
     * This routine is called by portaudio when playback is done.
     */
    void FSK_modulator::paStreamFinished(void* userData)
    {
        return ((FSK_modulator*)userData)->paStreamFinishedMethod();
    }


//This function is used to generate the output buffer for the paCallbackMethod function
//We will generate the symbols for FSK in here based on the input buffer
//We will keep track of a "state" variable
    void FSK_modulator::symbolgen(float* out, const void* inputbuffer,
		unsigned long framesPerBuffer)
    {
        unsigned long i;


	//Test code to generate an input sequence
	int testInput[framesPerBuffer];
	for(unsigned long j = 0; j < framesPerBuffer; j++) {
		if(j < 32)
			testInput[j] = 0;
		else
			testInput[j] = 1;
		//printf("%d\n", testInput[j]);
	}
 
        for( i=0; i<framesPerBuffer; i++ )
        {
	    printf("Output: %f, Input Bit: %d, Phase: %d\n", *(out-1), FSK_phase, phase);
	    //output buffer writes to mono output

    	    if(FSK_phase == 1)
                *out++ = sine0[phase];
    	    else
    		    *out++ = sine1[phase];  

            phase += 1;

            if( phase >= TABLE_SIZE ){ 
		//Reached the end of the current symbol
		//Move the current bit to the next symbol		

        		counter = (counter + 1)%framesPerBuffer;

        		//Reset the sinusoid
        		phase -= TABLE_SIZE;
        		
        		if(FSK_phase != testInput[counter])
        			printf("Output flipped!\n");
        		//FSK: Shift the phase by 180 degrees if the input signal changes
        		FSK_phase = testInput[counter];


	             }
    	}

     }








