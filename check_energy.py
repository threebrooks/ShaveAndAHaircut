import sys
import numpy as np
import math
import matplotlib.pyplot as plt

def is_event(x, y):
  max_rmse = 1/16.0
  max_speedup = 1.2

  if (len(x) != len(y)):
    return False
  N = len(x) 
  S_x = np.sum(x)
  S_y = np.sum(y)
  S_xy = np.sum(np.multiply(x,y))
  S_y2 = np.sum(np.multiply(y,y))
  C = (S_x*S_y-N*S_xy)/(S_y*S_y-N*S_y2)
  D = (C*S_y-S_x)/N
  y_tilde = C*y-D
  rmse = 0
  for x_idx, x_val in enumerate(x):
    rmse += (x_val-y_tilde[x_idx])*(x_val-y_tilde[x_idx])
  rmse /= N
  rmse = math.sqrt(rmse)
  if (rmse > max_rmse):
    return False
  speedup = (y[-1]-y[0])/(x[-1]-x[0])
  if (speedup > max_speedup or speedup < 1/max_speedup):
    return False
  return True

energy = np.load("energy.npz")
data_index = energy.files[0] 
data = energy[data_index][0]

thresh = 5
frame_step_rate = 5.33/1000.0
avg_bps = 150/60 # beats per second
avg_bpf = avg_bps*frame_step_rate
live_events = np.zeros(0)
inside_event = False
for data_idx, data_val in enumerate(data):
  if (data_val > thresh):
    if (not inside_event):
      live_events = np.append(live_events, data_idx*avg_bpf)
    inside_event = True
  else:
    inside_event = False

template = np.array([0.0, 1.0, 1.75, 2.0, 3.0])

for live_event_idx, live_event_time in enumerate(data):
  if (live_event_idx+len(template) > len(live_events)):
    break
  if (is_event(live_events[live_event_idx:], template)):
    print("Found event!")

display_data = data
plt.plot(range(len(display_data)),display_data)
plt.show()

