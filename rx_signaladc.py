import matplotlib.pyplot as plt
import scipy
from scipy.fftpack import fft
from scipy.signal import butter, lfilter, freqz, blackman
from scipy.io import wavfile
import numpy as np
import wave
import sys

# CONSTANTS
order = 3
cutoff = 1400;
freq1 = 1800
freq0 = 800

symbols_per_second = 24
sample_time_ms = 1000/symbols_per_second
samples_per_symbol = 22050/sample_time_ms

#Declaration of butterworth filters
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

#Signal plotting functions-- useful for debugging

def plot_timeDomain(x_time, y_signal, figTitle, figNum, x_min=0, x_max=1):
	plt.figure(figNum)
	plt.grid()

	plt.title(figTitle )
	plt.xlabel('Time')

	plt.xlim(xmin = x_min, xmax = x_max)
	plt.ylim(ymin=-4.5, ymax=4.5)

	plt.plot(x_time, y_signal, 'b')

def plot_freqDomain(data, fs, figTitle, figNum):
	plt.figure(figNum)
	signal_fft = fft(data)
	d = len(signal_fft)/2

	k = np.arange(len(data))
	T = len(data)/float(fs)
	frqLabel = k/T

	Xdb = 20 * np.log10(abs(signal_fft[: len(frqLabel)/2 ]))

	plt.plot(frqLabel[:len(frqLabel)/2], abs(signal_fft[: len(frqLabel)/2 ]), 'r')
	plt.title(figTitle)
	plt.xlim(xmin=0, xmax=4000)
	plt.ylim(ymin=0)

# Baseband conversion and signal detection
# i and q are quadrature (90 degrees out of phase) signals

def FSK_decision(signal, fs, freq0, freq1):
	t = np.linspace(0, len(signal)/float(fs), num=len(signal))
	order=6
	cos_mix0 = np.cos(freq0*2*np.pi*t)
	sin_mix0 = np.sin(freq0*2*np.pi*t)

	# plot_timeDomain(t, signal, "Plot of signal of " + filename + " in time domain", 3, 0,)
	# plot_freqDomain(signal, fs, "Plot of signal of " + filename + " in frequency domain", 8)

	i_0 = signal * cos_mix0
	q_0 = signal * sin_mix0

	i_0 = butter_lowpass_filter(i_0, 300, 2*fs, order)
	q_0 = butter_lowpass_filter(q_0, 300, 2*fs, order)

	i_sum0 = scipy.integrate.simps(abs(i_0))
	q_sum0 = scipy.integrate.simps(abs(q_0))

	Amplitude0 = np.sqrt(i_sum0**2 + q_sum0**2)

	cos_mix1 = np.cos(freq1*2*np.pi*t)
	sin_mix1 = np.sin(freq1*2*np.pi*t)

	i_1 = signal * cos_mix1
	q_1 = signal * sin_mix1

	i_1 = butter_lowpass_filter(i_1, 300, 2*fs, order)
	q_1 = butter_lowpass_filter(q_1, 300, 2*fs, order)

	# plot_timeDomain(t, i_0, "i_0", 6, 0, 0.05 )
	# plot_freqDomain(i_0, fs, "i_0", 7)

	# plot_timeDomain(t, i_1, "i_1", 4, 0, 0.05 )
	# plot_freqDomain(i_1, fs, "i_1", 5)

	i_sum1 = scipy.integrate.simps(abs(i_1))
	q_sum1 = scipy.integrate.simps(abs(q_1))

	Amplitude1 = np.sqrt(i_sum1**2 + q_sum1**2)

	# print "Amplitude 0: " + str(Amplitude0)
	# print "Amplitude 1: " + str(Amplitude1)

	out = Amplitude0 - Amplitude1
	if out > 0:
		res = 0
	if out < 0:
		res = 1
	return res

#Divide signal by its average-- goal is to minimize the differences in amplitude
def normalize_signal(data):
	mean_data = np.mean(abs(y))
	data_norm = [float(x/mean_data) for x in y]
	return data_norm

#Split up signal "data" into a set of samples, each with a length sample_time_ms
def sample(data, fs, sample_time_ms):
	#number of data points per symbol = len(data)/sampling rate / seconds per sample
	samples_per_symbol = fs/symbols_per_second
	symbCount = float(len(data))/samples_per_symbol

	y = [ [] for _ in range(int(symbCount))]
	for i in range(int(symbCount)):
		y[i] = data[i*(samples_per_symbol):(i+1)*samples_per_symbol]
	# print np.shape(y)
	return y

#TODO: Write a function to "trim" the wav file so silence is cut off
def trim_waveform(signal, fs):
	t = np.linspace(0, len(signal)/float(fs), num=len(signal))
	start = 0
	end   = -1
	for i in range(len(t)):
		if(abs(signal[i]) < 1):
			start = start + 1
		else:
			break
	
	for i in range(len(t)):
		if(abs(signal[-1-i]) < 1):
			end = end - 1
		else:
			break
	return start, end

#Timing recovery???


#Get WAV file, read into data
filename = str(sys.argv[1])
fs, data = wavfile.read(filename)
# data = data[0:(fs*1)] #Analyze only one second
#fs = fs/2 #Don't know why this works, but it works

Time = np.linspace(0, len(data)/(fs), num=len(data))
y = butter_lowpass_filter(data, cutoff, fs, order)

#normalize the amplitude of the signal
y_norm = normalize_signal(y)

start, end = trim_waveform(y_norm, fs)
y_trimmed = y_norm[start:end]
y_time = np.linspace(0, len(y_trimmed)/(fs), num=len(y_trimmed))

#break the signal into its constituent samples
sampled_data = sample(y_trimmed, fs, sample_time_ms)
remainder8 = len(sampled_data) % 8

if remainder8 > 0:
	samples_per_symbol = fs/symbols_per_second
	if remainder8 >= 4:
		for i in range(8-remainder8):
			sampled_data.append(y_trimmed[-1-samples_per_symbol:-1])
	else:
		sampled_data = sampled_data[:-remainder8]

print np.shape(sampled_data)


# plot_timeDomain(Time, data, "Plot of unfiltered signal of " + filename + " in time domain", 0)
# plot_timeDomain(y_time, y_trimmed, "Plot of filtered signal of " + filename + " in time domain", 1, 0.3, 0.9)
# #Plot of frequency spectrum of original file
# plot_freqDomain(data, fs, "Frequency Spectrum of " + filename + " without filtering", 2)
# plot_freqDomain(y_norm, fs, "Frequency Spectrum of " + filename + " with filtering", 3)

#Decision regions
#Using quadrature non-coherent detection
#Page 12 of http://www.electronics.dit.ie/staff/amoloney/lecture-26.pdf

result = []
with open('rx_msg.txt', 'w') as f:
	for n in range(len(sampled_data)):
		result.append(FSK_decision(sampled_data[n], fs, freq0, freq1))
		print >> f, result[n]
# plt.show()