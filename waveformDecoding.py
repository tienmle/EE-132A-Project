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

def butter_highpass(cutoff, fs, order=5):
    nyq = 0.5 * fs
    normal_cutoff = cutoff / nyq
    b, a = butter(order, normal_cutoff, btype='high', analog=False)
    return b, a

def butter_highpass_filter(data, cutoff, fs, order):
    b, a = butter_highpass(cutoff, fs, order=order)
    y = lfilter(b, a, data)
    return y

def butter_bandpass(lowf, highf, fs, order=5):
	nyq = 0.5 * fs
	low = lowf / nyq
	high = highf / nyq
	b, a = butter(order, [low, high], btype = 'band' )
	return b, a

def butter_bandpass_filter(data, cutoffL, cutoffH, fs, order=5):
	b, a = butter_bandpass(cutoffL, cutoffH, fs, order=order)
	y = lfilter(b, a, data)
	return y

def plot_timeDomain(x_time, y_signal, figTitle, figNum):
	plt.figure(figNum)
	plt.grid()

	plt.title(figTitle )
	plt.xlabel('Time')

	plt.xlim(xmin=0,xmax = 0.4)
	plt.ylim(ymin=-10000, ymax=10000)

	plt.plot(x_time, y_signal, 'b')

def plot_freqDomain(data, fs, figTitle, figNum):
	plt.figure(figNum)
	signal_fft = fft(data)
	d = len(signal_fft)/2

	k = np.arange(len(data))
	T = len(data)/fs
	frqLabel = k/T

	Xdb = 20 * np.log10(abs(signal_fft[: len(frqLabel)/2 ]))

	plt.plot(frqLabel[:len(frqLabel)/2], abs(signal_fft[: len(frqLabel)/2 ]), 'r')
	plt.title(figTitle)
	plt.xlim(xmin=0, xmax=2000)
	plt.ylim(ymin=0)

def FSK_decision(signal, fs, freq1, freq2):
	t = np.linspace(0, len(signal)/fs, num=len(data))
	cos_mix1 = np.cos(freq1*t)
	sin_mix1 = np.sin(freq1*t)

	i_1 = signal * cos_mix1
	q_1 = signal * sin_mix1
	i_sum1 = scipy.integrate.trapz(i_1)
	q_sum1 = scipy.integrate.trapz(q_1)

	Amplitude1 = np.sqrt(i_sum1**2 + q_sum1**2)

	cos_mix2 = np.cos(freq2*t)
	sin_mix2 = np.sin(freq2*t)

	i_2 = signal * cos_mix2
	q_2 = signal * sin_mix2
	i_sum2 = scipy.integrate.trapz(i_2)
	q_sum2 = scipy.integrate.trapz(q_2)

	Amplitude2 = np.sqrt(i_sum2**2 + q_sum2**2)

	out = Amplitude1 - Amplitude2
	if out > 0:
		res = 1
	if out < 0:
		res = 0
	return res


# CONSTANTS
#LPF Parameters
order = 10
cutoff = 1600

#BPF Parameters for center frequency of 1100
order = 3
cutoffH_1 = 1100
cutoffL_1 = 600

cutoffH_0 = 600
cutoffL_0 = 100

#Get WAV file, read into data
filename = str(sys.argv[1])

fs, data = wavfile.read(filename)
data = data[0:(fs*1.6)]


#Delete everything above range
def clip(data):
	for i in range(len(data)):
		if(data[i] > 100):
			data[i] = float(100.0)
		if(data[i] < -100):
			data[i] = float(-100.0)
	return data
#clip(data)
#plot of Time domain with filtered signal overlaid

Time = np.linspace(0, len(data)/fs, num=len(data))
y = butter_lowpass_filter(data, cutoff, 2*fs, order)


plot_timeDomain(Time, data, "Plot of unfiltered signal of " + filename + " in time domain", 0)
# plot_timeDomain(Time, y, "Plot of filtered signal of " + filename + " in time domain", 1)

# #Plot of frequency spectrum of original file
plot_freqDomain(data, fs, "Frequency Spectrum of " + filename + " without filtering", 2)
# plot_freqDomain(y, fs, "Frequency Spectrum of " + filename + " with filtering", 3)


#Plot of bandpass signals
#bit 1
y_bpf1 = butter_bandpass_filter(y, cutoffL_1, cutoffH_1, 2*fs, order)

#bit 0
y_bpf0 = butter_bandpass_filter(y, cutoffL_0, cutoffH_0, 2*fs, order)

# clip(y_bpf1)
# clip(y_bpf0)

# plot_timeDomain(Time, y_bpf1, "Plot of bandpass filtered signal of " + filename + " for bit = 1", 3)
# plot_timeDomain(Time, y_bpf0, "Plot of bandpass filtered signal of " + filename + " for bit = 0", 4)


#Plot of bandpass signals in frequency domain
y_bpf1_fft = fft(y_bpf1)
y_bpf0_fft = fft(y_bpf0)

# plot_freqDomain(y_bpf1_fft, fs, "Frequency Spectrum of " + filename + " with bandpass around bit=1", 5)
# plot_freqDomain(y_bpf0_fft, fs, "Frequency Spectrum of " + filename + " with bandpass around bit=0", 6)

#Decision regions
#Let's try using quadrature non-coherent detection
#Page 12 of http://www.electronics.dit.ie/staff/amoloney/lecture-26.pdf

print FSK_decision(y, fs, 550, 1100)

plt.show()