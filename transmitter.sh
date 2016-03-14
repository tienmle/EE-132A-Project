#!/bin/bash

make clean
make
echo "Transmitting message.."
./fsk_transmitter $1
