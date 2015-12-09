CPPFLAGS= -Wall -Os -O2 -Wvla-extension -std=gnu++11 -pedantic `pkg-config --cflags --libs jsoncpp`

all:
	ccache clang++ generate_distributions.cpp -o gdp $(CPPFLAGS)
	ccache clang++ bin_nfb_histogram.cpp -o bin_nfb $(CPPFLAGS)
	ccache clang++ un_normalize_data.cpp -o unnorm $(CPPFLAGS)
	ccache clang++ compute_running_moments.cpp -o rmom $(CPPFLAGS)

unnorm:
	ccache clang++ un_normalize_data.cpp -o unnorm $(CPPFLAGS)

rmom:
	ccache clang++ compute_running_moments.cpp -o rmom $(CPPFLAGS)

bin:
	ccache clang++ bin_nfb_histogram.cpp -o bin_nfb $(CPPFLAGS)

debug:
	ccache clang++ -v -g generate_distributions.cpp -o gdp $(CPPFLAGS)

clean:
	rm gdp
	rm bin_nfb
	rm unnorm
	rm rmom