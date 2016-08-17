#!/bin/bash
# is called like this
# alsa-stream <input-alsa-device> <output-alsa-device>
#
# keep this as low as possible without
# getting any underrun messages
# the bigger it is, the bigger the latency
RATE=48000
FORMAT=S16_LE
BUFFER_TIME=20000
arecord --rate=${RATE} --format=${FORMAT} --device=$1 --buffer-time=${BUFFER_TIME} | aplay --rate=${RATE} --format=${FORMAT} --device=$2 --buffer-time=${BUFFER_TIME} 
