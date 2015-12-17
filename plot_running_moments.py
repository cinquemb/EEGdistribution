import os
import sys
import math
import matplotlib.pyplot as plt
import matplotlib.cm as cm
from matplotlib.colors import LinearSegmentedColormap as LSC
from matplotlib.colorbar import ColorbarBase as CB
from matplotlib.colors import Normalize as NORM
import numpy as np
import datetime
import json
import msgpack

num_bins = 100
dir_data_name = '/Users/cinquemb/Documents/Startups/GoBlue/Research-Code/NFBcorrelationAnalysis/eeg_raw_distrubution_plots/nfb_histogram_data_real/nfb_running_moments/'
plot_dir_data_name = '/Users/cinquemb/Documents/Startups/GoBlue/Research-Code/NFBcorrelationAnalysis/eeg_raw_distrubution_plots/nfb_histogram_data_real/nfb_running_moments_plots/'
temp_dir_data = os.listdir(dir_data_name)
is_overall_ = False
cdict = {
	'red':[(0.0,1.0,1.0), (1.0,0.0,0.0)],
	'green': [(0.0,0.0,0.0), (1.0,0.0,0.0)],
	'blue': [(0.0,0.0,0.0), (1.0,1.0,1.0)]
}
red_blue_cm = LSC('RedBlue', cdict, 127)
colors = cm.get_cmap(red_blue_cm, 127)

def check_moment_graph_existence(_file_):
	global plot_dir_data_name
	moments_list = ['mean','kurt','stdv','skew']
	tally = 0
	for key in moments_list:
		if is_overall_:
			__check_png_file_name__ = 'nfb_running_moments_plots/' + str(key) + '_' + _file_.split('/')[-1].split('.json')[0] + '_overall_distrubution.png'
		else:
			__temp_png_dir_name__ = plot_dir_data_name + _file_.split('/')[-1].split('.json')[0]
			temp_new_dir = __temp_png_dir_name__ + '/' + str(key) + '/'
			__check_png_file_name__ = temp_new_dir + str(key) + '_' + _file_.split('/')[-1].split('.json')[0] + '_channel_plots.png'

		if os.path.isfile(__check_png_file_name__):
			tally+=1
	return True if tally == 4 else False

for x in temp_dir_data:
	if x != '.DS_Store':
		_file_ = dir_data_name+x
		if check_moment_graph_existence(_file_):
			print 'Pngs for moments exist:',_file_
			continue
		else:
			print 'Generating pngs for moments:',_file_

		try:
			jd = open(_file_, 'r+')
			json_data = msgpack.load(jd)
			jd.close()
		except:
			jd = open(_file_, 'r+')
			json_data = json.loads(jd.read())
			jd.close()

		if is_overall_:
			moments_dict = {}

			for i in range(2,129):
				for key, value in json_data[unicode(i)].iteritems():
					if key in  moments_dict:
						moments_dict[key] += value
					else:
						moments_dict[key] = value
					json_data[unicode(i)][key] = []

			for key,value in moments_dict.iteritems():
				__png_file_name__ = 'nfb_histogram_data_real/nfb_running_moments_plots/' + str(key) + '_' + _file_.split('/')[-1].split('.json')[0] + '_overall_distrubution.png'
				fig = plt.figure()
				fig.suptitle(key.title() + ' distribution (for ' +_file_.split('/')[-1].split('.json')[0] +')', fontsize=20)
				c,b,ba = plt.hist(value, num_bins, alpha=0.5)
				max_b = max(list(b))
				plt.xlabel("%s value" % (key))
				plt.ylabel("total %s values" % (key))
				plt.xlim([-max_b,max_b])
				fig.savefig(__png_file_name__)
				plt.close()
		else:
			inverted_moments_dict = {'mean':{},'kurt':{},'stdv':{},'skew':{}}

			for i in range(2,129):
				for key, value in json_data[unicode(i)].iteritems():
					inverted_moments_dict[key][i] = value
					json_data[unicode(i)][key] = []

			for mk, vk in inverted_moments_dict.iteritems():
				fig = plt.figure()
				fig.suptitle(mk.title() + ' TSA on channels (for ' +_file_.split('/')[-1].split('.json')[0] +')', fontsize=12)
				plt.xlabel("%s time bins" % (mk))
				plt.ylabel("%s value at time bin" % (mk))
				plt.xlim([0,len(vk[2])])
				__temp_png_dir_name__ = plot_dir_data_name + _file_.split('/')[-1].split('.json')[0]
				temp_new_dir = __temp_png_dir_name__ + '/' + str(mk) + '/'
				__png_file_name__ = temp_new_dir + str(mk) + '_' + _file_.split('/')[-1].split('.json')[0] + '_channel_plots.png'
				for i in range(2,129):
					x_index = range(0,len(vk[i]))
					plt.plot(x_index, vk[i], color=colors(i-2), hold=True)

				ax_cb = fig.add_axes([0.85,0.10,.05,.8])
				norm = NORM(vmin=2, vmax=128)
				cb = CB(ax_cb, cmap=red_blue_cm, norm=norm, orientation='vertical')
				fig.savefig(__png_file_name__)
				plt.close()

