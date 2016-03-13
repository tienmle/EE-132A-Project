CC = g++

#compiler flags
CFLAGS = -g -Wall

TARGET = audiomodem

default: $(TARGET)

$(TARGET): main.o waveformGen.o libportaudio.a
	$(CC) $(CFLAGS) main.o waveformGen.o libportaudio.a  -lrt -lm -lasound -pthread -o $(TARGET)

main.o: main.cpp waveformGen.h
	$(CC) $(CFLAGS) -c main.cpp

waveformGen.o:
	$(CC) $(CFLAGS) -c waveformGen.cpp

clean:
	$(RM) $(TARGET) *.o *~
