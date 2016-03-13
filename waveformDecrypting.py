#Written by Tien Le
#This module will contain the script to decode the message
#found by waveformDecode.py
filename = "rx_msg.txt"

b_content = []

with open(filename) as f:
    b_content = f.read().splitlines()

rx_ascii = ""
for i in b_content[:]:
	rx_ascii += i

print ''.join(chr(int(rx_ascii[i:i+8], 2)) for i in xrange(0, len(rx_ascii), 8))
