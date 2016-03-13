import matplotlib.pyplot as plt
import scipy
from scipy.fftpack import fft
from scipy.signal import butter, lfilter, freqz, blackman
from scipy.io import wavfile
import numpy as np
import wave
import sys

# CONSTANTS
#LPF Parameters
order = 10
cutoff = 1300

#BPF Parameters for center frequency of 1100
order = 3

freq1 = 900
cutoffH_1 = 1000
cutoffL_1 = 800
freq0 = 400
cutoffH_0 = 600
cutoffL_0 = 200

symbols_per_second = 24
sample_time_ms = 1000/symbols_per_second
samples_per_symbol = 22050/sample_time_ms
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

def plot_timeDomain(x_time, y_signal, figTitle, figNum, x_max=1):
	plt.figure(figNum)
	plt.grid()

	plt.title(figTitle )
	plt.xlabel('Time')

	plt.xlim(xmin=29,xmax = 30)
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

def FSK_decision(signal, fs, freq0, freq1):
	t = np.linspace(0, len(signal)/float(fs), num=len(signal))
	order=6
	cos_mix1 = np.cos(freq0*2*np.pi*t)
	sin_mix1 = np.sin(freq0*2*np.pi*t)
	# plot_timeDomain(t, signal, "Plot of signal of " + filename + " in time domain", 3)
	# plot_freqDomain(signal, fs, "Plot of signal of " + filename + " in frequency domain", 4)
	# print t
	# print signal
	i_1 = signal * cos_mix1
	q_1 = signal * sin_mix1

	i_1 = butter_lowpass_filter(i_1, 300, 2*fs, order)
	q_1 = butter_lowpass_filter(q_1, 300, 2*fs, order)

	i_sum1 = scipy.integrate.simps(abs(i_1))
	q_sum1 = scipy.integrate.simps(abs(q_1))

	Amplitude1 = np.sqrt(i_sum1**2 + q_sum1**2)

	cos_mix2 = np.cos(freq1*2*np.pi*t)
	sin_mix2 = np.sin(freq1*2*np.pi*t)

	i_2 = signal * cos_mix2
	q_2 = signal * sin_mix2


	i_2 = butter_lowpass_filter(i_2, 300, 2*fs, order)
	q_2 = butter_lowpass_filter(q_2, 300, 2*fs, order)

	# plot_timeDomain(t, i_1, "i_1", 6 )
	# plot_freqDomain(i_1, fs, "i_1", 7)

	# plot_timeDomain(t, i_2, "i_2", 4 )
	# plot_freqDomain(i_2, fs, "i_2", 5)

	i_sum2 = scipy.integrate.simps(abs(i_2))
	q_sum2 = scipy.integrate.simps(abs(q_2))

	Amplitude2 = np.sqrt(i_sum2**2 + q_sum2**2)

	# print "Amplitude 1: " + str(Amplitude1)
	# print "Amplitude 2: " + str(Amplitude2)

	out = Amplitude1 - Amplitude2
	if out > 0:
		res = 0
	if out < 0:
		res = 1
	return res

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
	for i in range(t):
		if(abs(signal[i]) < 0.2):
			start = start + 1
		else:
			break
	
	for i in range(t):
		if(abs(signal[-1-i]) < 0.2):
			end = end - 1
		else:
			break
	return start, end
	



#Timing recovery???


#Get WAV file, read into data
filename = str(sys.argv[1])

fs, data = wavfile.read(filename)
# data = data[0:(fs*1)] #Analyze only one second
fs = fs/2 #Don't know why this works, but it works

Time = np.linspace(0, len(data)/fs, num=len(data))
y = butter_lowpass_filter(data, cutoff, fs, order)

#normalize the amplitude of the signal
y_norm = normalize_signal(y)

#start, end = trim_waveform(y_norm)

#y_trimmed = y_norm[start,end]

#break the signal into its constituent samples
sampled_data = sample(y_norm, 2*fs, sample_time_ms)
print np.shape(sampled_data)
# plot_timeDomain(Time, data, "Plot of unfiltered signal of " + filename + " in time domain", 0)
plot_timeDomain(Time, y_norm, "Plot of filtered signal of " + filename + " in time domain", 1)

# #Plot of frequency spectrum of original file
# plot_freqDomain(data, fs, "Frequency Spectrum of " + filename + " without filtering", 2)
# plot_freqDomain(y_norm, fs, "Frequency Spectrum of " + filename + " with filtering", 3)


#Plot of bandpass signals
#bit 1
y_bpf1 = butter_bandpass_filter(y, cutoffL_1, cutoffH_1, fs, order)

#bit 0
y_bpf0 = butter_bandpass_filter(y, cutoffL_0, cutoffH_0, fs, order)

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
norm_data = normalize_signal(data)

result = []
file = open("rx_msg", "w")

for n in range(len(sampled_data)):
	result.append(FSK_decision(sampled_data[n], fs, freq0, freq1))
	file.write(chr(result[n-1]))

plt.show()
