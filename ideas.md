# Ideas #

- classifying distribution in data
	- list of classes of distrubition
	- find list of constants
		- https://en.wikipedia.org/wiki/Moment-generating_function
		- https://en.wikipedia.org/wiki/Cumulant
		- constants that need to be derived for each class (i.e mean, stdv; ignore arbitrary params)
			-mean
			-variance
			-skewness
			-kurtosis
	- defining a state density across subjects
		- un-normalize data across all subjects and recompute mean, stdv, skew, and kurtosis
			- iterate over nfb_histogram_metadata, load individual distrubution file, unormalize, save un-normalize data in map
			- play around with outliar removal etc? (sub #: 10,14,20,23,26,48,blank,DAE)
				- 10:
					-mean_all datasets (one channel off)
				- 14:
					-mean_all datasets (one channel off, except two with two off)
				- 20:
					-mean_ all but two(EMG runs look good) datasets (one channel off)
				- 23:
					-mean_23_RVIP_01_090814_1004_Run1_raw_bin_size_100_channel_plots.png (one channel off)
					-mean_23_10_EMG_01_090814_0954_Run1_raw_bin_size_100_channel_plots.png (one channel off)
					-mean_23_01_RS_01_090814_1019_Run1_raw_bin_size_100_channel_plots.png (one channel off)
				- 26:
					-mean_all datasets (one channel off)
				- 48:
					-mean_all datasets (one channel off)
				- blank:
					-mean_all datasets (one channel off)
				- DAE:
					-mean_all datasets (one channel off)
					
		- use values and compute running normalized mean, stdv, skew, kurtosis along each channel for each time series
			- figure out time bins (1/2048 of a second, to 1 second)
				- fixed length vector for each channel (armadillo matrix), zero initialized, that cycles through and stores the moment in order
			- generate plots/gifs
				- distrubitions for each moment
					~~- across all leads and bins (python|time-independant)~~
					- ~~plot each lead over time bin?~~ compute derivative? (c++/python| time dependent)
				- create tool to open moment for a random persons datasets to compare

- determine range of electropotential to help define hamiltonian (microcanoical ensamble) for along the scalp


# Notes #
- flat gausssian for mean distrubtion of voltages
- shallow quad modal stdv ""
- "" tri modal for skew ""
- approx dirac delta for kurt ~1, <3  (some guassian)

