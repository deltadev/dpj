CXX=clang++
CXXFLAGS=-O3 -std=c++11 -stdlib=libc++


all: dpj_utils.o
	ar -rvs $(HOME)/lib/libdpj.a dpj_utils.o #create or add file to archive
	ar -tv $(HOME)/lib/libdpj.a #list archive members
	cp dpj_utils.h dpj_utils.hh $(HOME)/include/

dpj_utils.o:
utils-tests/main: utils-tests/tests.cpp

copy:
	cp dpj_utils.h dpj_utils.hh prettyprint.hpp histogram.hpp $(HOME)/include/
run_test: 
	cd utils-tests && $(MAKE)
clean:
	rm -f dpj_utils.o test
