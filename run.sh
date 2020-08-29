#!/bin/bash -e

export JAVA_HOME=/usr/lib/jvm/java-11-openjdk-armhf

. ../Godec/install/env.sh
#. install/env.sh

export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:~/boost_1_74_0/stage/lib

. ./config.sh

if $event_logging 
then
  LOGS_DIR=/ramdisk
  for log_comp in audio_preproc process.!override.!window_0 process.!override.!chunk_energy_0 4mic
  do
    ADD_OPTS="$ADD_OPTS -x ${log_comp}.!log_file=$LOGS_DIR/${log_comp}.log -x ${log_comp}.verbose=true"
  done
fi

if $mic_audio_recording 
then
  echo 
else
  ADD_OPTS="$ADD_OPTS -x audio_writer_allchan=#audio_writer_allchan"
fi

REC_DEVNAME=$(arecord -L | grep -B 1 "seeed-4mic-voicecard" | grep "^hw")
REC_DEVIDX=$(cat /proc/asound/cards | grep "seeed-4mic-voicecard" | head -1 | awk '{print $1;}')
ADD_OPTS="$ADD_OPTS -x 4mic.soundcard_identifier=$REC_DEVNAME "

if ! grep -q Microsoft /proc/version ;
then
  CMD_PREFIX="nice -n -20 "
  for i in $(seq 0 3)
  do 
    sudo sh -c "echo performance > /sys/devices/system/cpu/cpu${i}/cpufreq/scaling_governor"
  done
fi

debug=false
if $debug
then
  CMD_PREFIX="$CMD_PREFIX gdb --args "
fi

$CMD_PREFIX godec -q -x "global_opts.SAMPLE_RATE=$SAMPLE_RATE" -x "global_opts.CAPTURE_CHUNK_SIZE=$CAPTURE_CHUNK_SIZE" -x "global_opts.CORRELATION_WINDOW_STEP_SIZE_MS=$CORRELATION_WINDOW_STEP_SIZE_MS" -x "global_opts.CORRELATION_WINDOW_SIZE_MS=$CORRELATION_WINDOW_SIZE_MS" -x "global_opts.OUTPUT_MODE=online" $ADD_OPTS online.json


