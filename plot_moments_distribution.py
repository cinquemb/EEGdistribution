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


f = open('nfb_histogram_data_real/nfb_histogram_data/nfb_histogram_metadata.json')
data = json.loads(f.read())
f.close()

savefig_path = ''
moments_dict = {}
num_bins = 250
for key,value in data.iteritems():
	for k,v in value.iteritems():
		if k in moments_dict:
			moments_dict[k].append(v)
		else:
			moments_dict[k] = [v]


for key,value in moments_dict.iteritems():		
	fig = plt.figure()
	r_vs = remove_outliars_beyond_sigma(value,1)
	fig.suptitle(key.title() + ' distribution (for ' +str(len(r_vs[0])) + ' files of '+str(len(value))+')', fontsize=20)
	c,b,ba = plt.hist(r_vs[0], num_bins, alpha=0.5)
	values_histo = list(c)
	mean_values = sum(r_vs[0])/float(len(r_vs[0]))
	mean_values_histo = sum(values_histo)/float(len(values_histo))
	max_b = max(list(b))
	min_b = min(list(b))
	width_vals = (max_b-min_b)
	real_min_b = min_b-width_vals
	stdv_ = get_stdv(values_histo, mean_values_histo)
	out_hist = 'file counts\n'+str(len(r_vs[0]))+'\ncounts(percent)\n'+','.join(map(str,list(c))) + '\nbins\n' + ','.join(map(str,list(b))) + '\nstandard_deviation\n' + str(stdv_) + '\nmean\n' + str(mean_values) + '\noutliars_removed_beyond_3_simga\n' + str(len(r_vs[1]))
	open(savefig_path+ str(key) + '_overall_distrubution.txt','w+').write(out_hist)
	plt.xlabel("%s value" % (key))
	plt.ylabel("total %s values" % (key))
	plt.xlim([real_min_b,max_b])
	fig.savefig(savefig_path+ str(key) + '_overall_distrubution.png')
	plt.close()