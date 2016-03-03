#ifndef WAVEFORMGEN_H
#define WAVEFORMGEN_H

#include "portaudio.h"

#define SAMPLE_RATE   (44100)
#define FRAMES_PER_BUFFER  (64)
#define TABLE_SIZE   (200)
#define AMPLITUDE 1

#ifndef M_PI
#define M_PI  (3.14159265)
#endif


class BPSK_modulator{

public:
	BPSK_modulator();
	bool open(PaDeviceIndex index);		
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
	
   	void buffergen(float* out, 
		const void* inputbuffer, unsigned long framesPerbuffer);

	//Member Variables
	PaStream *stream;
	float sine[TABLE_SIZE];
	int phase;
	char message[20];
	};

#endif
