import os
import sys
import math
import matplotlib.pyplot as plt
import matplotlib.cm as cm
import numpy as np
import datetime
import json
import msgpack
num_bins = 100

def check_moment_graph_existence(_file_):
	moments_list = ['mean','kurt','stdv','skew']
	tally = 0
	for key in moments_list:
		__check_png_file_name__ = 'nfb_running_moments_plots/' + str(key) + '_' + _file_.split('/')[-1].split('.json')[0] + '_overall_distrubution.png'
		if os.path.isfile(__check_png_file_name__):
			tally+=1
	return True if tally == 4 else False

dir_data_name = '/Users/cinquemb/Documents/Startups/GoBlue/Research-Code/NFBcorrelationAnalysis/eeg_raw_distrubution_plots/nfb_histogram_data_real/nfb_running_moments/'
temp_dir_data = os.listdir(dir_data_name)



for x in temp_dir_data:
	if x != '.DS_Store':
		_file_ = dir_data_name+x
		if check_moment_graph_existence(_file_):
			print 'Distribution pngs for moments exist:',_file_
			continue
		else:
			print 'Generating distribution pngs for moments:',_file_

		try:
			jd = open(_file_, 'r+')
			json_data = msgpack.load(jd)
			jd.close()
		except:
			jd = open(_file_, 'r+')
			json_data = json.loads(jd.read())
			jd.close()

		moments_dict = {}

		for i in range(2,129):
			for key, value in json_data[unicode(i)].iteritems():
				if key in  moments_dict:
					moments_dict[key] += value
				else:
					moments_dict[key] = value
				json_data[unicode(i)][key] = []

		for key,value in moments_dict.iteritems():
			__png_file_name__ = 'nfb_running_moments_plots/' + str(key) + '_' + _file_.split('/')[-1].split('.json')[0] + '_overall_distrubution.png'
			fig = plt.figure()
			fig.suptitle(key.title() + ' distribution (for ' +_file_.split('/')[-1].split('.json')[0] +')', fontsize=20)
			c,b,ba = plt.hist(value, num_bins, alpha=0.5)
			max_b = max(list(b))
			plt.xlabel("%s value" % (key))
			plt.ylabel("total %s values" % (key))
			plt.xlim([-max_b,max_b])
			fig.savefig(__png_file_name__)
			plt.close()