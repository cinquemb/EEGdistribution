import os
import sys
import subprocess

dir_data_name = '/Volumes/dae/nfb_running_moments/'
temp_dir_data = os.listdir(dir_data_name)

def copy_files(file_):
	cmd = 'cp %s /Users/cinquemb/Documents/Startups/GoBlue/Research-Code/NFBcorrelationAnalysis/eeg_raw_distrubution_plots/nfb_histogram_data_real/nfb_running_moments/' % (file_)
	try:
		proc = subprocess.Popen(cmd, shell=True, stdin=None, stdout=subprocess.PIPE, stderr=None, close_fds=True)
		proc_data = proc.communicate()[0]
		data = True
	except Exception, e:
		data = False

	return data


def nullify_files(file_):
	cmd = 'cat /dev/null > %s' % (file_)
	try:
		proc = subprocess.Popen(cmd, shell=True, stdin=None, stdout=subprocess.PIPE, stderr=None, close_fds=True)
		proc_data = proc.communicate()[0]
		data = True
	except Exception, e:
		data = False

	return data



for x in temp_dir_data:
	if x != '.DS_Store':
		rf = dir_data_name + x
		if os.stat(rf).st_size == 0:
			print 'Already Nulled:', x
		else:
			#copy file
			rc_val = copy_files(rf)
			if rc_val == True:
				print 'Copy Success:', x
				#null file
				rval = nullify_files(rf)
				if rval == True:
					print 'Nulled:', x
				else:
					print 'Nulled FAILED:', x
					sys.exit()
			else:
				print 'Copy FAILED:', x
				sys.exit()