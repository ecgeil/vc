SHELL=/bin/sh
CXX=g++
CXXFLAGS=-Wall -std=c++11 -O3
source=vc.cpp
executable=vc
install_dir=/usr/local/bin

all: 
	$(CXX) $(CXXFLAGS) -o $(executable) $(source)

.PHONY: clean
clean:
	rm -f $(executable)

install: $(executable)
	cp -n $(executable) $(install_dir)/$(executable)

.PHONY: check
check: $(executable)
	( cd tests ; ./runtests.sh )