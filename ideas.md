# Ideas #

- read file probabilsitically into mem based on size to parse faster
	- use lead_reduction_line_mapping_code
	- for each read, measure distanse between newlines found in order to inform (on avg or markovly) next bit offset from last newline and reset pointer to last newline position + 1

- classifying distribution in data
	- list of classes of distrubition
	- find list of constants
		- https://en.wikipedia.org/wiki/Moment-generating_function
		- https://en.wikipedia.org/wiki/Cumulant
		- constants that need to be derived for each class (i.e mean, stdv; ignore arbitrary params)
			-mean
			-variance
			-skewness ()

