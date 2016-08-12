#!/bin/bash
# is called like this
# alsa-stream <input-alsa-device> <output-alsa-device>
#
# keep this as low as possible without
# getting any underrun messages
# the bigger it is, the bigger the latency
BUFFER_TIME=20000
arecord --rate=48000 --format=S16_LE --device=$1 --buffer-time=${BUFFER_TIME} | aplay --rate=48000 --format=S16_LE --buffer-time=${BUFFER_TIME} --device=$2
