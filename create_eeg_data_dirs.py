import os
import sys
import math

is_create = False
dir_data_name = '/Users/cinquemb/Documents/Startups/GoBlue/Research-Code/NFBcorrelationAnalysis/eeg_raw_distrubution_plots/nfb_histogram_data_real/nfb_running_moments/' 
plot_dir_data_name = '/Users/cinquemb/Documents/Startups/GoBlue/Research-Code/NFBcorrelationAnalysis/eeg_raw_distrubution_plots/nfb_histogram_data_real/nfb_running_moments_plots/'
temp_dir_data = os.listdir(dir_data_name)
moments_list = ['mean','kurt','stdv','skew']
for x in temp_dir_data:
	if x != '.DS_Store':
		_file_ = dir_data_name+x
		__temp_png_dir_name__ = plot_dir_data_name + _file_.split('/')[-1].split('.json')[0]
		if is_create:
			os.mkdir(__temp_png_dir_name__)
			for key in moments_list:	
				os.mkdir(__temp_png_dir_name__ + '/' + str(key))
		else:
			for key in moments_list:
				__png_file_name_old__ = plot_dir_data_name + str(key) + '_' + _file_.split('/')[-1].split('.json')[0] + '_overall_distrubution.png'
				temp_new_dir = __temp_png_dir_name__ + '/' + str(key) + '/'
				__png_file_name_new__ = temp_new_dir + str(key) + '_' + _file_.split('/')[-1].split('.json')[0] + '_overall_distrubution.png'
				os.rename(__png_file_name_old__, __png_file_name_new__)