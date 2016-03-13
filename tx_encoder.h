#ifndef TXENCODER_h
#define TXENCODER_H

//Implementation of a R = 1/2 convolutional encoder
//Static class-- can directly call member functions in code
class encoder{

public:
	encoder();
	~encoder();

	static void conv12Encoder(char* msg, char* &encoded_msg);
};

#endif
