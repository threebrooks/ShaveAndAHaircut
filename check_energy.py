import sys
import numpy as np
import math
import matplotlib.pyplot as plt

energy = np.load("energy.npz")
data_index = energy.files[0] 
data = energy[data_index][0]
log_data = np.log(data)
display_data = data
plt.plot(range(len(display_data)),display_data)
plt.show()

