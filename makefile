CXX=clang++
CXXFLAGS=-O3 -std=c++11 -stdlib=libc++

all: archive copy

archive: dpj_utils.o net.o
	ar -rvs $(HOME)/lib/libdpj.a dpj_utils.o net.o #create or add file to archive
	ar -tv $(HOME)/lib/libdpj.a #list archive members

dpj_utils.o:
utils-tests/main: utils-tests/tests.cpp

copy:
	cp dpj_utils.h dpj_utils.hh histogram.hh indexer.hh order.hh \
           prettyprint.hpp stats.hh $(HOME)/include/
run_test: 
	cd utils-tests && $(MAKE)
clean:
	rm -f dpj_utils.o test
