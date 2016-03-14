
#include <stdio.h>
#include <math.h>
#include "portaudio.h"
#include "tx_waveformGen.h"

//For transforming ASCII text into binary
#include <string>
#include <bitset>
#include <iostream>
#include <fstream>

#include <cassert>

using namespace std;

#define NUM_SECONDS 20
/*******************************************************************/
int main(int argc, char* argv[]);


//To run this program, compile using make and run with a text file as an argument

int main(int argc, char* argv[])
{
    if(argc < 2){
        cout << "Please include a textfile to transmit!" << endl;
        return -1;
    }

    std::string file_name = argv[1];
    std::ifstream file(file_name.c_str());

    if(!file){
        cout << "Unable to open file " + file_name << endl;
        return -1;
    }

    string message;
    string temp;
    if(getline(file,temp)){
        message = temp;
    }
    while(getline(file,temp)){
        message += '\n' + temp;
    }
    //Take message from string and transform into binary (ASCII)
    string binary_message; // raw text data

    for (std::size_t i = 0; i < message.size(); i++){
        bitset<8> bit_string(message.c_str()[i]);
        binary_message += bit_string.to_string();
    }

    //Test code to sanity check turning string file into bits
    // assert(binary_message.size() % 8 == 0); //Sanity check
    // string recovered_message;
    // //Now back to ASCII
    // for(std::size_t i = 0; i < binary_message.size(); i += 8){
    //     std::bitset<8> char_msg(binary_message.substr(i, i+7));
    //     recovered_message += char(char_msg.to_ulong());
    // }
    //cout << recovered_message << endl;

    // PortAudio Code
    printf("PortAudio: output FSK Signal. Sample rate = %d, BufSize = %d\nMessage Size:%u\n", 
        SAMPLE_RATE, FRAMES_PER_BUFFER, (unsigned)message.size());

    PaError err;
    FSK_modulator fsk(binary_message);
    
    err = Pa_Initialize();
    if( err != paNoError ) goto error;

    if (fsk.open(Pa_GetDefaultOutputDevice()))
    {
        if (fsk.start())
            {
                while(fsk.IsStreamActive() == 1)
                    Pa_Sleep(100);
            }

        fsk.stop();
        fsk.close();
    }

    Pa_Terminate();
    printf("Transmission finished.\n");
    
    return err;

error:
    Pa_Terminate();
    fprintf( stderr, "An error occured while using the portaudio stream\n" );
    fprintf( stderr, "Error number: %d\n", err );
    fprintf( stderr, "Error message: %s\n", Pa_GetErrorText( err ) );
    return err;
}
