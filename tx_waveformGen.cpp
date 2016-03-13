
//TX2 Design
// 3/1/2016
// Written by: Tien Le

//Waveform mapping using FSK Modulation
//Take a carrier sinusoid and modulate its phase depending on whether the input bit is a 1 or a 0

#include "tx_waveformGen.h"

    FSK_modulator::FSK_modulator(std::string binarymessage) : stream(0), phase(0)
    {
        /* initialise sinusoidal wavetable for 0 and 1 */

        //Duration of pulse is Table_size/samplerate
        for( int i=0; i<TABLE_SIZE; i++ )
        {
            //printf("i = %d\n", i);
            sine1[i] = (float)(AMPLITUDE1 * sin( ((double)i/(double)TABLE_SIZE) * M_PI * 2.* FREQ_1 ));
            sine0[i] = (float)(AMPLITUDE0 * sin( ((double)i/(double)TABLE_SIZE) * M_PI * 2.* FREQ_0 ));
        }

        //Create data structure for holding the encoded message
        //Size = 2 * size of binary message + 4 padding bits at the end + 1 null byte
        char* tx_encoded_message = new char[2*strlen(binarymessage.c_str())+ 4 + 1];

        encoder::conv12Encoder((char*) binarymessage.c_str(), tx_encoded_message);
        tx_message = tx_encoded_message;
        msg_size = strlen(tx_message);

        //**Initialiation of the preamble**
        //We will pad out the preamble with a series of alternating 0 and 1 symbols
        //This is intended to 'wake up' the
        //To aid with synchronization, we will utilize a 5-bit barker code {+1 +1 +1 0 +1}
        tx_preamble = new char[PREAMBLE_SIZE+1];
        for(int i = 0; i < (PREAMBLE_SIZE - 5); i++){
            tx_preamble[i] = (i % 2) + '0';
        }
        tx_preamble[PREAMBLE_SIZE-5] = 1  + '0';
        tx_preamble[PREAMBLE_SIZE-4] = 1  + '0';
        tx_preamble[PREAMBLE_SIZE-3] = 1  + '0';
        tx_preamble[PREAMBLE_SIZE-2] = 0  + '0';
        tx_preamble[PREAMBLE_SIZE-1] = 1  + '0';
        tx_preamble[PREAMBLE_SIZE] = '\0';

        //Initialization of the output state
        state = 2; // State 2: Play preamble, State 1: Play message, State 0: Stop playing stream
        FSK_symbol = tx_preamble[0] - '0';
        counter = 1;

        //DEBUGGING 
        printf("Preamble\n%s", tx_preamble);
        printf( "\nSize of preamble message: %u\n", (unsigned)strlen(tx_preamble));

        printf("%s", tx_message);
        printf( "\nSize of encoded message message: %u\n", (unsigned)strlen(tx_message));

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

    bool FSK_modulator::IsStreamActive(){
        return Pa_IsStreamActive(stream);
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

        //Stop transmitting when we're done
        if( state == 0){
            return paComplete;
        }

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
        printf( "Stream Completed.\n");
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
	//Test code to generate an input sequence-- good for pure testing pure sinusoids

    	// int testInput[framesPerBuffer];
    	// for(unsigned long j = 0; j < framesPerBuffer; j++) {
    	// 	if(j % 2 == 1)
    	// 		testInput[j] = 0;
    	// 	else
    	// 		testInput[j] = 1;
    	// 	//printf("%d\n", testInput[j]);
    	// }

        //Set a symbol to play for a certain amount of time
        //symbol_length is the amount of time each symbol will be played for
        //ms per symbol = 1000 ms / symbols/sec
        //This is set by multiplying sample rate (samples/sec) * desired time
        int symbol_length = SAMPLE_RATE/SYMBOLS_PER_SECOND;
        for( i=0; i<framesPerBuffer; i++ )
        {
	        //printf("Output: %f, Input Bit: %d, Phase: %d\n", *(out-1), FSK_symbol, phase);

            //Play sound if message is either sending its preamble or the actual message

            if(state == 1 || state == 2) {
                if(FSK_symbol == 0)
                    *out++ = sine0[phase];
        	    else
        		    *out++ = sine1[phase]; 
                phase++;
            }
            if(state == 0) {
                *out++ = 0;
            }

            if( phase >= symbol_length){ 
        		//Reached the end of the current symbol, play the next symbol
        		//Reset the phase of the sinusoid

                //printf("Reached end of table, counter is = %d\n", counter);

                if( state == 2)
                {
                    if( (unsigned)counter == PREAMBLE_SIZE )
                    {
                        state = 1;
                        FSK_symbol = tx_message[0] - '0';
                        counter = 1;
                    }
                    phase -= symbol_length;
                    FSK_symbol = tx_preamble[counter] - '0';
                    counter++;
                }
                if( state == 1 ) 
                {
                    if( (unsigned)counter > msg_size )
                    {
                        state = 0;
                    }
                    phase -= symbol_length;
                    FSK_symbol = inputbuffer[counter] - '0';
                    counter++;
                }

                // printf("%d", FSK_symbol);
                // FSK_symbol = testInput[counter+1];
                // counter = counter + 1 %framesPerBuffer;
                // printf("Output symbol: %d, %d symbols left to print\n", FSK_symbol, msg_size - counter );

	             }
    	}

     }








