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

#Next state where rows represent 00, 01, 10, 11 and columns represent inputs of 0 or 1
nextState = [[0,2],[0,2], [1,3], [1,3]]
#Encoder output table where indices represent prev inputs 00, 01, 10, 11 with an input of 0 or 1
encOutput = [[0,3], [3,0], [2,1], [1,2]]
#Input table #Rows represent 00, 01, 10, 11 and columns represent next state with same ordering
#Table indicates what input can lead from transition from current state to next state
#For example, if I wanted to know how 00 can lead to 10, I look up inputTable[0][3] and see that I input 1
# -1 means no possible transition
inputTable = [["0","-1", "1", "-1"],["0","-1", "1", "-1"],["-1","0", "-1", "1"],["-1","0", "-1", "1"]] 

#Enumerates states into an integer
def stateIndex(state):
	if(state == "00"):
		return 0
	if(state == "01"):
		return 1		
	if(state == "10"):
		return 2
	if(state == "11"):
		return 3
	return -1

hammingTable00 = [ 0, 1, 1, 2 ]
hammingTable01 = [ 1, 0, 2, 1 ]
hammingTable10 = [ 1, 2, 0, 1 ]
hammingTable11 = [ 2, 1, 1, 0 ]
index = [hammingTable00, hammingTable01, hammingTable10, hammingTable11]

#Finds the hamming distance (# of different symbols between two signals) 
def hammingDist(next_state, received_symb):
	return index[next_state][received_symb]

def viterbiDecoder(rx_ascii):
	index = 0
	errorMetric = 1000 * np.ones((4,len(rx_ascii)/2+1))
	errorMetric[0][0] = 0

	errIndex = 1
	for i in xrange(0,len(rx_ascii),2):
		receivedSymb = stateIndex(rx_ascii[i:i+2])
		for j in xrange(4):
			n1 = nextState[j][0]
			out1 = encOutput[j][0]
			a = hammingDist(out1, receivedSymb)
			if(errorMetric[n1][errIndex] >= a + errorMetric[j][errIndex-1] ):
				errorMetric[n1][errIndex] = a + errorMetric[j][errIndex-1]
			n2 = nextState[j][1]
			out2 = encOutput[j][1]
			b = hammingDist(out2, receivedSymb)
			if(errorMetric[n2][errIndex] >= b + errorMetric[j][errIndex-1] ):
				errorMetric[n2][errIndex] = b + errorMetric[j][errIndex-1]
		errIndex += 1
	#Backtrack through the array
	msg = ""
	#Array of the index of each state that minimizes the accumulated error
	minErrorIndex = np.argmin(errorMetric, axis=0)
	for k in xrange(1,len(minErrorIndex)):
		msg = inputTable[minErrorIndex[-k-1]][minErrorIndex[-k]] + msg
	# Message decoded, cut off 2 padding bits at the end
	msg = msg[:-1]
	return msg

#BEGINNING OF SCRIPT
file_content = []
with open(filename) as f:
    file_content = f.read().splitlines()
#Create the string
rx_ascii = ""
for i in file_content[:]:
	rx_ascii += i
#Strip the preamble off, look for the barker code and then start decoding past there
message_start = stripPreamble(rx_ascii)
decoded_msg = viterbiDecoder(rx_ascii[message_start:])

#Decode from binary

recovered_msg = ''.join(chr(int(decoded_msg[i:i+8], 2)) for i in xrange(0, len(decoded_msg), 8))
with open('received_msg.txt', 'w') as f:
		print >> f, recovered_msg
