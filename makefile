CC = g++

#compiler flags
CFLAGS = -g -Wall

TARGET = audiomodem

default: $(TARGET)

$(TARGET): main.o tx_waveformGen.o tx_encoder.o libportaudio.a
	$(CC) $(CFLAGS) main.o tx_waveformGen.o tx_encoder.o libportaudio.a -lrt -lm -lasound -pthread -o $(TARGET)

main.o: main.cpp tx_waveformGen.h
	$(CC) $(CFLAGS) -c main.cpp

tx_waveformGen.o:
	$(CC) $(CFLAGS) -c tx_waveformGen.cpp

tx_encoder.o:
	$(CC) $(CFLAGS) -c tx_encoder.cpp

clean:
	$(RM) $(TARGET) *.o *~
