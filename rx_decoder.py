#Written by Tien Le
#This module will contain the script to decode the message
#found by waveformDecode.py

import numpy as np

filename = "rx_msg.txt"

#Removes the padding in the beginning
def stripPreamble(rx_ascii):
	barker5 = "11101"
	pos = 0
	for i in range(len(rx_ascii)-5):
		if(rx_ascii[i:i+5] == barker5):
			pos = i+5
			break;
	return pos

# K = 3 (7,5 convolutional code decoder)
# Encoder used the following function for the R=1/2 encoder
# y0 = x0 + x-1 + x-2
# y1 = x0 + x-2
# Initial state was {0,0}, last two symbol pairs were generated with {0,0}

K = 3
nextState = np.zeros(shape=(4,2))
nextState[0] = ["00","10"] 	 #00
nextState[1] = ["00","10"] 	 #01
nextState[2] = ["01","11"] 	 #10
nextState[3] = ["01","11"] 	 #11

print nextState
# def viterbidecoder(rx_ascii):
	# trellis = []
	# trellis.append([]) # 00
	# trellis.append([]) # 01
	# trellis.append([]) # 10
	# trellis.append([]) # 11
	# trellis[0].append(0) #Initialized state is 00
	# trellis[1].append(-1)
	# trellis[2].append(-1)
	# trellis[3].append(-1)
	# index = 0
	# prevState = "0" #initial state was 00
	# for i in xrange(1,len(rx_ascii),1):
	# 	curState = rx_ascii[index:index+2]
	# 	if(prevState = "0"): # 00
	# 		if(curState = "00"):
	# 			trellis[0][i] = trellis[0][i-1] #bit 0 
	# 			trellis[2][i] = trellis[0][i-1] #bit 1
	# 		if(curState = "11"):
	# 			trellis[0][i] = trellis[i-1]
	# 	if(prevState = "1"): # 01
	# 		continue
	# 	if(prevState = "2"): # 10
	# 		continue
	# 	if(prevState = "3"): # 11
	# 	index += 2

b_content = []

with open(filename) as f:
    b_content = f.read().splitlines()
#Create the string
rx_ascii = ""
for i in b_content[:]:
	rx_ascii += i
#Strip the preamble off, look for the barker code and then start decoding past there
message_start = stripPreamble(rx_ascii)
rx_ascii = rx_ascii[message_start:]
viterbidecoder(rx_ascii)
# print rx_ascii
# print len(rx_ascii)

#Decode

# print ''.join(chr(int(rx_ascii[i:i+8], 2)) for i in xrange(0, len(rx_ascii), 8))
