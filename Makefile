
CC=g++
CXXFLAGS=-std=c++20

all: test othello benchmark

othello: main.cpp
	$(CC) $(CXXFLAGS) $< -o $@

test: test.cpp
	$(CC) $(CXXFLAGS) $< -o $@
	./test

benchmark: benchmark.cpp
	$(CC) $(CXXFLAGS) $< -o $@
	./benchmark

clean:
	rm -rf *.o
	rm -rf othello
	rm -rf test