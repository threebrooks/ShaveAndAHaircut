import sys
import godec
import numpy as np
import gc
import subprocess
import time

class ShaveAndAHaircut:

  def __init__(self, rec_device=None):
    ovs = godec.overrides()
  
    sample_rate = 48000
    ovs.add("global_opts.SAMPLE_RATE",str(sample_rate))
  
    capture_chunk_size = 1024
    ovs.add("global_opts.CAPTURE_CHUNK_SIZE",str(capture_chunk_size))
  
    energy_window_size = 1024
    energy_window_size_ms = 1000.0*energy_window_size/sample_rate
    ovs.add("global_opts.ENERGY_WINDOW_SIZE_MS",str(energy_window_size_ms))
    #print("Energy window size "+str(energy_window_size)+"ms")
  
    energy_window_step_factor = 4
    energy_window_step_size_ms = energy_window_size_ms/energy_window_step_factor
    ovs.add("global_opts.ENERGY_WINDOW_STEP_SIZE_MS",str(energy_window_step_size_ms))
    #print("Energy window step size "+str(energy_window_step_size_ms)+"ms")
  
    decay_ms = 1000*10; # 10 seconds decay
    decay_num_frames = int(decay_ms/energy_window_step_size_ms)
    ovs.add("global_opts.DECAY_NUM_FRAMES",str(decay_num_frames))
   
    if (not rec_device): 
      rec_device = subprocess.Popen('arecord -L | grep "^hw" | head -1', shell=True, stdout = subprocess.PIPE).stdout.read().decode().strip()
    ovs.add("mic.soundcard_identifier",rec_device)

    self.utt_count = -1
    
    push_endpoints = godec.push_endpoints()
    push_endpoints.add("soundcard_control")
    
    pull_endpoints = godec.pull_endpoints()
    godec.load_godec("online.json", ovs, push_endpoints, pull_endpoints, True)
 
def start_listening(): 
    self.utt_count += 1
    conv_message = godec.conversation_state_decoder_message(0, "utt_id"+str(self.utt_count), False, "convo_id"+str(self.utt_count), False)
    godec.push_message("soundcard_control", conv_message)

def stop_listening():
    conv_message = godec.conversation_state_decoder_message(1, "utt_id"+str(self.utt_count), True, "convo_id"+str(self.utt_count), True)
    godec.push_message("soundcard_control", conv_message)

def shutdown(): 
    godec.blocking_shutdown()
