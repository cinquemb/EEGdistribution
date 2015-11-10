CPPFLAGS= -Wall -Os -O2 -Wvla-extension -std=gnu++11 -pedantic `pkg-config --cflags --libs jsoncpp`
all:
	ccache clang++ generate_distributions.cpp -o gdp $(CPPFLAGS)
debug:
	ccache clang++ -v -g generate_distributions.cpp -o gdp $(CPPFLAGS)

clean:
	rm gdp