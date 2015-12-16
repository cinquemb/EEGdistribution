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
			- play around with outliar removal etc?
		- use values and compute running normalized mean, stdv, skew, kurtosis along each channel for each time series
			- figure out time bins (1/2048 of a second, to 1 second)
				- fixed length vector for each channel (armadillo matrix), zero initialized, that cycles through and stores the moment in order
			- generate plots/gifs
				- distrubitions for each moment
					~~- across all leads and bins (python|time-independant)~~
					- across each lead channel (python|time-independant)
					- plot each lead over time bin? compute derivative? (c++| time dependent)

- determine range of electropotential to help define hamiltonian (microcanoical ensamble) for along the scalp


# Notes #
- flat gausssian for mean distrubtion of voltages
- shallow quad modal stdv ""
- "" tri modal for skew ""
- approx dirac delta for kurt ~1, <3  (some guassian)

