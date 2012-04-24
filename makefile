CXXFLAGS=-O3 -std=c++11
all: Utils.o
	ar -rvs $(HOME)/lib/libdpj.a Utils.o
	ar -tv $(HOME)/lib/libdpj.a
	cp Utils.h Utils.hpp $(HOME)/include/
test:
	g++ test.cpp -o test -ldpj
	./test

clean:
	rm -f dpj_utils.o test
