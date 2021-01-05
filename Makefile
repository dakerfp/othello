
CC=g++
CXXFLAGS=-std=c++20 -O3 -Wall
VERSION=`git rev-parse --short HEAD`

all: test othello benchmark

othello: main.cpp
	$(CC) $(CXXFLAGS) -DVERSION=\"$(VERSION)\" $< -o $@

test: test.cpp
	$(CC) $(CXXFLAGS) $< -o $@
	./test
	rm -rf test

benchmark: benchmark.cpp
	$(CC) $(CXXFLAGS) $< -o $@
	./benchmark 10

clean:
	rm -rf *.o
	rm -rf othello
	rm -rf test
	rm -rf benchmark