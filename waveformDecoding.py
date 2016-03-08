import matplotlib.pyplot as plt
import scipy
from scipy.fftpack import fft
from scipy.signal import butter, lfilter, freqz, blackman
from scipy.io import wavfile
import numpy as np
import wave
import sys



#Declaration of filters
def butter_lowpass(cutoff, fs, order=5):
    nyq = 0.5 * fs
    normal_cutoff = cutoff / nyq
    b, a = butter(order, normal_cutoff, btype='low', analog=False)
    return b, a

def butter_lowpass_filter(data, cutoff, fs, order):
    b, a = butter_lowpass(cutoff, fs, order=order)
    y = lfilter(b, a, data)
    return y

#LPF Parameters
order = 6
cutoff = 1500

#Get WAV file
filename = str(sys.argv[1])

fs, data = wavfile.read(filename)
data = data[0:(fs*1.6)]
# data = wave.open(filename, 'r')
# signal = data.readframes(-1)
# signal = np.fromstring(signal, 'Int16')
# fs =	 data.getframerate()
#Plot of frequency spectrum of original file
signal_fft = fft(data)
d = len(signal_fft)/2

Xdb = 20 * np.log10(abs(signal_fft))
k = np.arange(len(data))
T = len(data)/fs
frqLabel = k/T


plt.figure(2)
plt.plot(frqLabel, Xdb)
plt.title('Frequency Spectrum of ' + filename )
plt.xlim(xmin=0, xmax = 2000)
plt.ylim(ymin=0)


#plot of Time domain with filtered signal overlaid
plt.figure(1)
plt.subplot(2,1,2)
plt.grid()

plt.title('Output of ' + filename )
plt.xlabel('Time')

plt.xlim(xmin=0)
plt.ylim(ymin=-7000, ymax=7000)

Time = np.linspace(0, len(data)/fs, num=len(data))

y = butter_lowpass_filter(data, cutoff, 2*fs, order)

plt.plot(Time,data)
plt.plot(Time, y)


# Butterworth filter characteristics
# e,filter_coefficient = butter_lowpass(cutoff, fs, order)

# plt.figure(3)
# w, h = freqz(e, filter_coefficient, worN=8000)
# plt.subplot(2, 1, 1)
# plt.plot(0.5*fs*w/np.pi, np.abs(h), 'b')
# plt.plot(cutoff, 0.5*np.sqrt(2), 'ko')
# plt.axvline(cutoff, color='k')
# plt.xlim(0, 0.5*fs)
# plt.title("Lowpass Filter Frequency Response")
# plt.xlabel('Frequency [Hz]')
# plt.grid()

#plot FFT of filtered signal
plt.figure(3)
c_lpf = fft(y)
d_lpf = len(c_lpf)/2

k_lpf = np.arange(len(data))
T_lpf = len(data)/fs
frqLabel_lpf = k/T

X2db = 20*np.log10(abs(c_lpf))

plt.plot(frqLabel_lpf, X2db, 'red')
plt.title('Frequency Spectrum of ' + filename + ' with filtering' )
plt.xlim(xmin=0, xmax = 2000)
plt.ylim(ymin=0)

#plot FFT of filtered signal at HF
bins = 10


test = np.empty([bins, int(len(y)/bins)], dtype=complex)
result = np.empty([bins, int(len(y)/bins)], dtype=complex)
print fs
test[0,:] = y[ 0 : int(len(y)/bins) ]

for i in range(10):
	test[i, :] = y[i*(int(len(y)/bins)) : (i+1)*int(len(y)/bins) ]

w = blackman(int(len(y)/bins))

for i in range(3):
	result[i, :] = fft(test[i,:]*w)
	plt.figure(4+i)
	k_lpf = np.arange(len(result[i]))
	T_lpf = len(result[i])/fs
	frqLabel_lpf = k/T
	X2db = 20*np.log10(abs(result[i]))
	#X2db /= np.max(np.abs(X2db),axis=0)
	# print len(result[i])
	# print T_lpf
	plt.plot(k_lpf, X2db, 'red')
	plt.title('Frequency Spectrum of subset ' + str(i) + ' of ' + filename + ' with filtering' )
	plt.xlim(xmin=0, xmax = 2000)
	plt.ylim(ymin=0)
	plt.ylabel("Power (dB)")
	plt.xlabel("Frequency (Hz)")



#Investigate this
testdata = y[0: fs*0.8]
Time = np.linspace(0, float(len(testdata))/fs, num=len(testdata))
plt.figure(10)
plt.plot(Time, testdata)
plt.xlim(xmin=0)

blackman_test = blackman(len(testdata))

plt.figure(11)


testfft = fft(testdata)
X2db = 20*np.log10(abs(testfft))
plt.plot(np.arange(len(testfft)), abs(testfft))

plt.title('Frequency Spectrum of subset HF of ' + filename + ' with filtering' )
plt.xlim(xmin=0)
plt.ylim(ymin=0)
plt.ylabel("Power (dB)")
plt.xlabel("Frequency (Hz)")

plt.show()