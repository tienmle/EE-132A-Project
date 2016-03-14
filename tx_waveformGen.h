#ifndef WAVEFORMGEN_H
#define WAVEFORMGEN_H

#include "portaudio.h"
#include "tx_encoder.h"
#include "tx_waveformGen.h"

#include <stdio.h>
#include <iostream>
#include <math.h>
#include <cassert>

#include <string>
#include <cstring>

#define FRAMES_PER_BUFFER  (128)

//Each sine table can play 2 times per second
#define SAMPLE_RATE   (22050)
#define TABLE_SIZE   (11025)
#define AMPLITUDE0 0.7
#define AMPLITUDE1 9
#define SYMBOLS_PER_SECOND 24
#define FREQ_0 800.
#define FREQ_1 1800.
#define PREAMBLE_SIZE 12

#ifndef M_PI
#define M_PI  (3.14159265)
#endif


class FSK_modulator{

public:
	FSK_modulator(std::string binary_message);
	~FSK_modulator();
	bool open(PaDeviceIndex index);	
	bool IsStreamActive();
	bool close();
	bool start();
	bool stop();
	
private:
	int paCallbackMethod(const void *inputBuffer, 
		void *outputBuffer,
		unsigned long framesPerBuffer,
		const PaStreamCallbackTimeInfo* timeInfo,
		PaStreamCallbackFlags statusFlags);

	static int paCallback( const void *inputBuffer, 
		void *outputBuffer,
		unsigned long framesPerBuffer,
		const PaStreamCallbackTimeInfo* timeInfo,
		PaStreamCallbackFlags statusFlags,
		void *userData );

	void paStreamFinishedMethod();
	static void paStreamFinished(void* userData);
	
   	void symbolgen(float* out, 
		const char* inputbuffer, unsigned long framesPerbuffer);

	//Member Variables
	PaStream *stream;
	

	//Message Variable (stored as a C-string)
	char* tx_preamble;
	char* tx_message;
	size_t msg_size;
	size_t tx_pos;


	//Sine Tables
	float sine0[TABLE_SIZE];
	float sine1[TABLE_SIZE];
	int phase;
	
	//FSK Implementation
	int state;
	int counter;
	bool FSK_symbol;

	char message[20];
	};

#endif
