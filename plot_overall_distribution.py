import os
import sys
import math
import matplotlib.pyplot as plt
import matplotlib.cm as cm
import numpy as np
import datetime
import json
sys.path.insert(0, '/Users/cinquemb/Documents/Startups/GoBlue/Research-Code/NFBcorrelationAnalysis/')
from calculate_correlation import get_correlation, get_stdv, remove_outliars_beyond_sigma


f = open('nfb_histogram_data_real/nfb_histogram_data/nfb_histogram_binned_500.json')
data = json.loads(f.read())
f.close()

print 'done loading file'

savefig_path = ''
x = []
y = []
for key,value in data.iteritems():
	x.append(float(key))
	y.append(value)

print 'done parsing file'
max_x = max(list(x))

fig = plt.figure()
fig.suptitle('raw eeg voltage distribution normalized', fontsize=20)
plt.scatter(x, y)
plt.xlim([-max_x,max_x])
plt.xlabel("Normed values")
plt.ylabel("Value occurance")
fig.savefig('overall_distrubution.png')
plt.close()