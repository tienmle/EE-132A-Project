#!/bin/bash

echo Testing Transmitter and Receiver
make clean && make
rm rx_msg.txt

./fsk_transmitter msg.txt & python rx_frontend.py

python rx_signaladc.py output.wav
python rx_decoder.py
