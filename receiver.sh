#!/bin/bash

echo Testing Receiver
rm rx_msg.txt

python rx_frontend.py
python rx_signaladc.py output.wav
python rx_decoder.py

cat received_msg.txt
