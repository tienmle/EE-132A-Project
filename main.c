
#include <stdio.h>
#include <math.h>
#include "portaudio.h"
#include "waveformGen.h"

//For transforming ASCII text into binary
#include <string>
#include <bitset>
#include <iostream>
#include <fstream>

#include <cassert>

using namespace std;

#define NUM_SECONDS 2
/*******************************************************************/
int main(int argc, char* argv[]);


//To run this program, compile using make and run with a text file as an argument

int main(int argc, char* argv[])
{
    std::string file_name = argv[1];
    std::ifstream file(file_name.c_str());

    if(!file){
        cout << "Unable to open file " + file_name << endl;
        return -1;
    }

    string message;
    string temp;
    while(getline(file,temp)){
        message += '\n' + temp;
    }
    //cout << message << endl;

    PaError err;
    FSK_modulator fsk;

    //Take message from string and transform into binary (ASCII)
    string binary_message; // raw text data

    for (std::size_t i = 0; i < message.size(); i++){
        bitset<8> bit_string(message.c_str()[i]);
        binary_message += bit_string.to_string();
        //cout << binary_message << endl;
    }


    //Test code to sanity check turning string file into bits
    assert(binary_message.size() % 8 == 0); //Sanity check
    string recovered_message;
    //Now back to ASCII
    for(std::size_t i = 0; i < binary_message.size(); i += 8){
        std::bitset<8> char_msg(binary_message.substr(i, i+7));
        //cout << char(char_msg.to_ulong()) << endl;
        recovered_message += char(char_msg.to_ulong());
    }
    //cout << recovered_message << endl;

    // PortAudio Code
    printf("PortAudio: output FSK Signal. SR = %d, BufSize = %d\n", SAMPLE_RATE, FRAMES_PER_BUFFER);
    
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
