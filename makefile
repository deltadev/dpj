CXX=clang++
CXXFLAGS=-O3 -std=c++11 -stdlib=libc++


all: Utils.o
	ar -rvs $(HOME)/lib/libdpj.a Utils.o #create or add file to archive
	ar -tv $(HOME)/lib/libdpj.a #list archive members
	cp Utils.h Utils.hpp $(HOME)/include/

Utils.o:
utils-tests/main: utils-tests/tests.cpp

run_test: 
	cd utils-tests && $(MAKE)
clean:
	rm -f Utils.o test
