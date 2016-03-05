#ifndef WAVEFORMGEN_H
#define WAVEFORMGEN_H

#include "portaudio.h"

#define SAMPLE_RATE   (44010)
#define FRAMES_PER_BUFFER  (128)
// Output frequency ~= Sample Rate / Frames_Per_Buffer
// The output frequency isn't exactly represented by the sample above, need to determine why
#define TABLE_SIZE   (128)
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
	
   	void symbolgen(float* out, 
		const void* inputbuffer, unsigned long framesPerbuffer);

	//Member Variables
	PaStream *stream;
	float sine0[TABLE_SIZE];
	float sine1[TABLE_SIZE];
	int phase;
	//BPSK Implementation
	int prev_input;
	int counter;
	bool BPSK_phase;
	char message[20];
	};

#endif
