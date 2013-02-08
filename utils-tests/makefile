CXX=clang++
CXXFLAGS=-O3 -std=c++11 -stdlib=libc++
LDFLAGS=-L${HOME}/lib -ldpj 
CPPFLAGS=-I${HOME}/include

run: main
	./main

tests.o:

main: tests.o
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -o main main.cpp tests.o $(LDFLAGS)
