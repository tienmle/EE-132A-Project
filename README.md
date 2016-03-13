# EE-132A-Project

The objective of this project is to design a wireless communication system using
a microphone as a receiver and computer speakers as the transmitter.

Design Specifications:

1. Bit rate >= 5 bps
2. Errors in transferred text <= 2%
3. Transmitter/Receiver pairs should be spaced at least 10 meters apart

Design notes-

Transmitter

The transmitter implemented using the Portaudio c++ library. The Sine template was 
adapted into a binary FSK system where symbol 0 is 400 Hz and symbol 1 is 900 Hz
(this is configurable in the waveformGen.h file). The symbol rate is 24 symbols per
second, which with convolutional coding is roughly 12 bits of data per second.

To use, call ./audiomodem FILENAME where FILENAME is an ASCII text file. After
reading the text into memory, the string is converted into a string of binary
representing each character (8 binary characters per ASCII character). This is
then passed into a R=1/2 (5,7) convolutional coder.

Convolutional Coder Design: (tx_encoder.cpp)
The design of the convolutional coder is a straightforward implementation of the
following function:

y_0 = b_0 + b_1 + b_2

y_1 = b_0 + b_2

where + represents modulo 2 addition (xor).

The effective length of the new signal is twice the number of symbols of the old
signal plus 4 symbols. The sequence '00' is padded at the beginning and the end in 
order to ensure full performance of the encoder. 

Receiver

Because of the ease of implementation of signal processing libraries (numpy,
fft, etc.), the receiver is written entirely in Python. Capturing the signal is
a python implementation of the portaudio library, pyAudio. This is all done in
rx_frontend.py.

In rx_signaladc.py, the signal is split up into its component symbols
(approximately, signal synchronization is always an issue). For a sequence 's'
seconds long with a symbol transmission rate of 'm' symbols, there will be m*s
samples analyzed. The script will output a list of binary symbols observed from
the signal.

A quadrature demodulator was implemented for determing the symbol of each sample. 

The concept is as follows--

For sinusoidal signal f0 and signal f1:

cos(f0 t) * cos(f0 t) = 1/2 (1 + cos(2f0 t))
sin(f0 t) * sin(f0 t) = 1/2 (1 - sin(2f0 t))

By multiplying the signal in quadrature (90 degrees out of phase) with a
frequency that corresponds to symbol 0 and symbol 1, we can integrate the square
of the signal near the baseband and compare the two to see which frequency is 
"stronger". By applying a low pass filter with a low cutoff frequency, we can
observe which of the frequencies has a larger component around the baseband.

After the symbols have been recovered from each of the samples, it is a matter
of reconstructing the signal from the list of symbols. rx_decoder.py is
the Python script that uses Viterbi's algorithm to recover the original signal.


TO-DO: 
-Receiver Synchronization in the presence of noise (Barker code)
-ISI Compensation (Raised cosine filter/hamming window)
-Implementation of convolutional decoder
-Receiver 'standby' (Detect incoming signal, closely related to Rx synchroniation)




