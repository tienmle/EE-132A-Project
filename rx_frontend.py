import pyaudio
import audioop
import wave

CHUNK = 512
FORMAT = pyaudio.paInt16
CHANNELS = 1
RATE = 22050
THRESHOLD = 800
#RECORD_SECONDS = 30
WAVE_OUTPUT_FILENAME = "output.wav"

p = pyaudio.PyAudio()

stream = p.open(format=FORMAT,
                channels=CHANNELS,
                rate=RATE,
                input=True,
                frames_per_buffer=CHUNK)

print("Receiver on standby, waiting for a signal above threshold")

frames = []
while True:
#	for i in range(0, int(RATE / CHUNK * RECORD_SECONDS)):
    data = stream.read(CHUNK)
    rms = audioop.rms(data,2) #Get root mean square of the chunk
    if (rms > THRESHOLD):
    	print("Signal detected! Recording...")
    	#print data
    	frames.append(data) #Add the signal we just found to the wav file
    	while (rms > THRESHOLD):
    		data=stream.read(CHUNK)
    		frames.append(data)
    		rms = audioop.rms(data,2)
		data=stream.read(CHUNK)
		frames.append(data) #Read one last chunk, we can cut this out later
    else:
    	continue
    break;

print("Done recording")

stream.stop_stream()
stream.close()
p.terminate()

wf = wave.open(WAVE_OUTPUT_FILENAME, 'wb')
wf.setnchannels(CHANNELS)
wf.setsampwidth(p.get_sample_size(FORMAT))
wf.setframerate(RATE)
wf.writeframes(b''.join(frames))
wf.close()
