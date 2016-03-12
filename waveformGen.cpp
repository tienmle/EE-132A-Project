
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
#include <string>
#include <cstring>
#include <iostream>

    FSK_modulator::FSK_modulator(std::string binarymessage) : stream(0), phase(0),toggle(0), counter(0), FSK_symbol(false) 
    {
	    //assert(TABLE_SIZE%2 == 0);
        /* initialise sinusoidal wavetable for 0 and 1 */

        //Duration of pulse is Table_size/samplerate
        for( int i=0; i<TABLE_SIZE; i++ )
        {
            //printf("i = %d\n", i);
            sine1[i] = (float)(AMPLITUDE * sin( ((double)i/(double)TABLE_SIZE) * M_PI * 2.* 1000. ));
            sine0[i] = (float)(AMPLITUDE * sin( ((double)i/(double)TABLE_SIZE) * M_PI * 2.* 500.));
        }

        //Generate the container for the message to be sent
        tx_message = new char[binarymessage.length()];

        for (size_t index = 0; index < binarymessage.length(); index++){
            tx_message[index] = binarymessage[index];
        }
        tx_message[binarymessage.length()] = '\0';
        msg_size = strlen(tx_message);
        //printf("%s", tx_message);
        sprintf( message, "No Message" );
    }

    FSK_modulator::~FSK_modulator(){
        delete tx_message;
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

        //Hacky, but we will just directly use the tx_message object here

	    symbolgen(out, tx_message, framesPerBuffer);
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
    void FSK_modulator::symbolgen(float* out, const char* inputbuffer,
		unsigned long framesPerBuffer)
    {
        unsigned long i;
	//Test code to generate an input sequence

    	int testInput[framesPerBuffer];
    	for(unsigned long j = 0; j < framesPerBuffer; j++) {
    		if(j % 2 == 1)
    			testInput[j] = 1;
    		else
    			testInput[j] = 0;
    		//printf("%d\n", testInput[j]);
    	}
        
        //Set a symbol to play for a certain amount of time
        //symbol_length to be set to 5 ms
        int symbol_length = SAMPLE_RATE*double(50)/1000;
        for( i=0; i<framesPerBuffer; i++ )
        {
	    //printf("Output: %f, Input Bit: %d, Phase: %d\n", *(out-1), FSK_symbol, phase);
	    //output buffer writes to mono output
            //printf("%f\n", sine0[phase]);
            if(FSK_symbol == 0)
                *out++ = sine0[phase];
    	    else
    		    *out++ = sine1[phase];  

            phase++;

            if( phase >= symbol_length ){ 
		//Reached the end of the current symbol
		//Move the current bit to the next symbol		
                printf("Reached end of table, counter is = %d\n", counter);
        		//Reset the sinusoid
        		phase -= symbol_length;
        		
        		// if(FSK_symbol != inputbuffer[counter] - '0')

        		// 	//printf("Output flipped!\n");

        		// FSK_symbol = inputbuffer[counter] - '0';
          //       //Counter will keep looping the message
          //       counter = (counter + 1)%msg_size;


                FSK_symbol = testInput[counter];
                counter = (counter + 1) %framesPerBuffer;
                //printf("Output symbol: %d, %d symbols left to print\n", FSK_symbol, msg_size - counter );

	             }
    	}

     }








