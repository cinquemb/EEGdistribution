import os
import sys
import subprocess

def open_outliars(file_list):
	cmd = 'open %s' % (' '.join(file_list))
	try:
		proc = subprocess.Popen(cmd, shell=True, stdin=None, stdout=subprocess.PIPE, stderr=None, close_fds=True)
		proc_data = proc.communicate()[0]
		data = True
	except Exception, e:
		data = False

	return data



moments_index = int(sys.argv[1])

plot_dir_data_name = '/Users/cinquemb/Documents/Startups/GoBlue/Research-Code/NFBcorrelationAnalysis/eeg_raw_distrubution_plots/nfb_histogram_data_real/nfb_running_moments_plots/'
temp_dir_data = os.listdir(plot_dir_data_name)
moments_list = ['mean','stdv','skew','kurt']
outliars = ['10','14','20','26','32','48','blank','DAE']
exclude_list = ['23_RVIP_01_090814_1004_Run1_raw_bin_size_100', '23_10_EMG_01_090814_0954_Run1_raw_bin_size_100', '23_01_RS_01_090814_1019_Run1_raw_bin_size_100']
white_list = ['20_10_EMG_02_082214_1019_Run1_raw_bin_size_100','20_10_EMG_01_082214_1008_Run1_raw_bin_size_100']
#outliar_init = [x + '_' for x in outliars]

#mean_10_1_RS_01_080514_1048_Run1_raw_bin_size_1000_channel_plots.png

explore_files_dict = {x:[] for x in outliars}
explore_files_dict['23'] = []
filtered_files = []
for x in temp_dir_data:
	if x != '.DS_Store':
		#aproxx filter, may break
		subject = x.split('_')[0]
		real_data_file = x.split('_bin_size_100')[0] + '.txt'

		if x in white_list:
			continue
		elif x in exclude_list:
			filtered_files.append(real_data_file)
			_t_moment_file = plot_dir_data_name + x + '/'+moments_list[moments_index] +'/' + moments_list[moments_index] + '_' + x + '_channel_plots.png'
			explore_files_dict[subject].append(_t_moment_file)
		elif any(subject == y for y in outliars):
			if x[-3:] == '100':
				_t_moment_file = plot_dir_data_name + x + '/'+moments_list[moments_index] +'/' + moments_list[moments_index] + '_' + x + '_channel_plots.png'
				explore_files_dict[subject].append(_t_moment_file)
				filtered_files.append(real_data_file)

open('nfb-mined_file_list_filtered.txt','w+').write(','.join(filtered_files))

#for k,v in explore_files_dict.iteritems():
#	open_outliars(v)
