
#include <stdio.h>
#include <math.h>
#include "portaudio.h"
#include "waveformGen.h"

#define NUM_SECONDS 2
/*******************************************************************/
int main(void);
int main(void)
{
    PaError err;
    FSK_modulator fsk;

    printf("PortAudio Test: output sine wave. SR = %d, BufSize = %d\n", SAMPLE_RATE, FRAMES_PER_BUFFER);
    
    err = Pa_Initialize();
    if( err != paNoError ) goto error;

    if (fsk.open(Pa_GetDefaultOutputDevice()))
    {
        if (fsk.start())
        {
            printf("Play for %d seconds.\n", NUM_SECONDS );
            Pa_Sleep( NUM_SECONDS * 1000 );

            fsk.stop();
        }

        fsk.close();
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
