
#include <stdio.h>
#include <math.h>
#include "portaudio.h"
#include "waveformGen.h"

#define NUM_SECONDS 5
/*******************************************************************/
int main(void);
int main(void)
{
    PaError err;
    BPSK_modulator bpsk;

    printf("PortAudio Test: output sine wave. SR = %d, BufSize = %d\n", SAMPLE_RATE, FRAMES_PER_BUFFER);
    
    err = Pa_Initialize();
    if( err != paNoError ) goto error;

    if (bpsk.open(Pa_GetDefaultOutputDevice()))
    {
        if (bpsk.start())
        {
            printf("Play for %d seconds.\n", NUM_SECONDS );
            Pa_Sleep( NUM_SECONDS * 1000 );

            bpsk.stop();
        }

        bpsk.close();
    }

    Pa_Terminate();
    printf("Test finished.\n");
    
    return err;

error:
    Pa_Terminate();
    fprintf( stderr, "An error occured while using the portaudio stream\n" );
    fprintf( stderr, "Error number: %d\n", err );
    fprintf( stderr, "Error message: %s\n", Pa_GetErrorText( err ) );
    return err;
}
