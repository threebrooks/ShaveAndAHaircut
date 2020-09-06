import sys
import godec
import numpy as np
import gc
import subprocess

def runme():
  ovs = godec.overrides()

  sample_rate = 48000
  ovs.add("global_opts.SAMPLE_RATE",str(sample_rate))

  capture_chunk_size = 64
  ovs.add("global_opts.CAPTURE_CHUNK_SIZE",str(capture_chunk_size))

  energy_window_size = 1024
  energy_window_size_ms = 1000.0*energy_window_size/sample_rate
  ovs.add("global_opts.ENERGY_WINDOW_SIZE_MS",str(energy_window_size_ms))
  print("Energy window size "+str(energy_window_size)+"ms")

  energy_window_step_factor = 4
  energy_window_step_size_ms = energy_window_size_ms/energy_window_step_factor
  ovs.add("global_opts.ENERGY_WINDOW_STEP_SIZE_MS",str(energy_window_step_size_ms))
  print("Energy window step size "+str(energy_window_step_size_ms)+"ms")

  decay_ms = 1000*10; # 10 seconds decay
  decay_num_frames = int(decay_ms/energy_window_step_size_ms)
  ovs.add("global_opts.DECAY_NUM_FRAMES",str(decay_num_frames))
  print("Decay # frames"+str(decay_num_frames))
  
  mic_audio_recording=False
  if (not mic_audio_recording):
    ovs.add("audio_writer_allchan","#audio_writer_allchan")
  
  rec_devname = subprocess.Popen('arecord -L | grep -B 1 "seeed-4mic-voicecard" | grep "^hw"', shell=True, stdout = subprocess.PIPE).stdout.read().decode().strip()
  rec_devidx = subprocess.Popen('cat /proc/asound/cards | grep "seeed-4mic-voicecard" | head -1 | awk "{print $1;}"',shell=True, stdout = subprocess.PIPE).stdout.read().decode().strip()
  ovs.add("4mic.soundcard_identifier",rec_devname)
  
  push_endpoints = godec.push_endpoints()
  push_endpoints.add("soundcard_control")
  
  pull_endpoints = godec.pull_endpoints()
  #pull_endpoints.add("pull_test", ["preproc_audio", "energy"])
  
  godec.load_godec("online.json", ovs, push_endpoints, pull_endpoints, True)

  recording = False
  timestamp = -1
  for line in sys.stdin:
    recording = not recording
    timestamp += 1
    conv_message = godec.conversation_state_decoder_message(timestamp, "utt_id"+str(timestamp), not recording, "convo_id"+str(timestamp), not recording)
    godec.push_message("soundcard_control", conv_message)
  
#  num_samples = 16000000
#  data = np.random.randn(num_samples).astype('c')
#  binary_message = godec.binary_decoder_message(num_samples-1, data, "sample_rate=16000")
#  godec.push_message("raw_audio", binary_message)
#  
#  conv_message = godec.conversation_state_decoder_message(num_samples-1, "utt_id", True, "convo_id", True)
#  godec.push_message("convstate", conv_message)
#  
#  msg_block = godec.pull_message("pull_test", 1000)
#  print("new sample rate: "+str(msg_block["preproc_audio"].sample_rate))
#  
  godec.blocking_shutdown()
  
runme()

