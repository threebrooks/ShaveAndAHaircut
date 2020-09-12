import sys
import numpy as np
import math
import matplotlib.pyplot as plt

def is_event(x, y):
  max_rmse = 1/16.0
  max_speedup = 1.5

  if (len(x) != len(y)):
    return False
  N = len(x) 
  S_x = np.sum(x)
  S_y = np.sum(y)
  S_xy = np.sum(np.multiply(x,y))
  S_yy = np.sum(np.multiply(y,y))
  C = (S_x*S_y-N*S_xy)/(S_y*S_y-N*S_yy)
  D = (C*S_y-S_x)/N
  #print("C: "+str(C)+", D: "+str(D))
  y_tilde = C*y-D
  x_tilde = (x+D)/C
  rmse = 0
  print("x: "+str(x))
  print("y: "+str(y))
  for x_idx, x_val in enumerate(x):
    rmse += (x_val-y_tilde[x_idx])*(x_val-y_tilde[x_idx])
  rmse /= N
  rmse = math.sqrt(rmse)
  speedup = (y[-1]-y[0])/(x[-1]-x[0])
  print("rmse: "+str(rmse) + "speedup: "+str(speedup))
  if (rmse > max_rmse):
    return False
  if (speedup > max_speedup or speedup < 1/max_speedup):
    return False
  return True

energy = np.load("energy.npz")
data_index = energy.files[0] 
data = energy[data_index][0]
#data = data[-400:]

thresh = 1
frame_step_rate = 5.33/1000.0
avg_bps = 150/60 # beats per second
print("frame_step_rate: "+str(frame_step_rate))
avg_bpf = avg_bps*frame_step_rate
print("avg_bpf: "+str(avg_bpf))
live_events = np.zeros(0)
inside_event = False
inside_max_idx = -1
inside_max_energy = -1E30
for data_idx, data_val in enumerate(data):
  if (data_val > thresh):
    inside_event = True
    if (data_val > inside_max_energy):
      inside_max_energy = data_val
      inside_max_idx = data_idx
  elif (data_val < 0.5*thresh):
    if (inside_event):
      live_events = np.append(live_events, inside_max_idx*avg_bpf)
    inside_max_energy = -1E30
    inside_event = False

template = np.array([0.0, 1.0, 1.65, 2.0, 3.0])

for live_event_idx, live_event_time in enumerate(data):
  if (live_event_idx+len(template) > len(live_events)):
    break
  if (is_event(live_events[live_event_idx:live_event_idx+len(template)], template)):
    print("Found event!")

display_data = data
plt.plot(np.linspace(0.0, len(display_data)*avg_bpf,len(display_data)),display_data)
plt.show()

