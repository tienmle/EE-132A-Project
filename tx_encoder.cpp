//Convolutional Encoding Module
//Written by Tien Le

//Implementation of a R=1/2 code
//The design was taken from http://users.ece.utexas.edu/~gerstl/ee382v-ics_f09/lectures/Viterbi.pdf
#include <cstring>
#include <stdio.h>
#include "tx_encoder.h"

	encoder::encoder(){
		return;
	}
	encoder::~encoder(){
		return;
	}

	//K=3, m=2
	//2 output symbols per 1 input symbol
	//(7,5) convolutional code:
	// y0 = x0 + x-1 + x-2 (modulo addition)
	// y1 = x0 + x-3
	void encoder::conv12Encoder(char* msg, char* &encoded_msg){
		//We add 2 more pairs of symbols at the end to "flush" the encoder
		//This is to ensure that the last
		int state[2] = {0,0};
		int current_state = msg[0];
		int n = 0;
		for(size_t j = 1; j < strlen(msg); j++){
			char y0 = (current_state + state[0] + state[1]) % 2 +'0';
			char y1 = (current_state + state[1]) % 2 + '0';
			encoded_msg[n++] = y0;
			encoded_msg[n++] = y1;

			state[1] = state[0];
			state[0] = current_state;
			current_state = msg[j];
		}
		//We've encoded all messages up to the very last character
		//Pad it out with two more inputs of 0 so we can recover everything
		char y0 = (current_state + state[0] + state[1]) % 2 + '0';
		char y1 = (current_state + state[1]) % 2 + '0';

		encoded_msg[n++] = y0;
		encoded_msg[n++] = y1;

		state[1] = state[0];
		state[0] = current_state;
		current_state = 0;

		encoded_msg[n++] = y0;
		encoded_msg[n++] = y1;


		y0 = (current_state + state[0] + state[1]) % 2 + '0';
		y1 = (current_state + state[1]) % 2 + '0';

		state[1] = state[0];
		state[0] = current_state;
		current_state = 0;

		y0 = (current_state + state[0] + state[1]) % 2 +'0';
		y1 = (current_state + state[1]) % 2 + '0';

		encoded_msg[n++] = y0;
		encoded_msg[n++] = y1;

		//Finish c-string
		encoded_msg[n] = '\0';

		return;
	}