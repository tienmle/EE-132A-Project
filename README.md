# EE-132A-Project

The objective of this project is to design a wireless communication system using
a microphone as a receiver and computer speakers as the transmitter.

Design Specifications:

1. Bit rate >= 5 bps
2. Errors in transferred text <= 2%
3. Transmitter/Receiver pairs should be spaced at least 10 meters apart

Design notes-

Transmitter implemented using portaudio c++ library. Sine template was adapted
into a binary FSK system where symbol 0 is 400 Hz and symbol 1 is 900 Hz
(Configurable in the waveformGen.h file).

To use, call ./audiomodem FILENAME where FILENAME is an ASCII text file. After
reading the text into memory, the string is converted into a string of binary
representing each character (8 binary characters per ASCII character). This is
then passed into a R=1/2 (5,7) convolutional coder.

Convolutional Coder
The design of the convolutional coder is a straightforward implementation of the
following function:

y_0 = b_0 + b_1 + b_2
y_1 = b_0 + b_2

The effective length of the new signal is twice the number of symbols of the old
signal plus 4 symbols. The sequence '00' is padded at the end in order to ensure
full error correction.

Receiver

Because of the ease of implementation of signal processing libraries (numpy,
fft, etc.), the receiver is written entirely in Python. Capturing the signal is
a python implementation of the portaudio library, pyAudio. This is all done in
waveformRec.py.

In waveformDecoding.py, the signal is split up into its component symbols
(approximately, signal synchronization is always an issue). For a sequence 's'
seconds long with a symbol transmission rate of 'm' symbols, there will be m*s
samples analyzed.

For determing the symbol of each sample, a quadrature demodulator was
implemented. The theory is as follows--

For sinusoidal signal f0 and signal f1:

cos(f0 t) * cos(f0 t) = 1/2 (1 + cos(2f0 t))
sin(f0 t) * sin(f0 t) = 1/2 (1 - sin(2f0 t))

By multiplying the signal in quadrature (90 degrees out of phase) with a
frequency that corresponds to symbol 0 and symbol 1, we can integrate the square
of the signal and compare the two to see which frequency is more 'prevalent' in
the sample.

After the symbols have been recovered from each of the samples, it is a matter
of reconstructing the signal from the list of symbols. waveformDecrypting.py is
the Python script that uses Viterbi's algorithm to recover the original signal.







