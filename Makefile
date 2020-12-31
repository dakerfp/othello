
CC=g++
CXXFLAGS=-std=c++17

all: test othello

othello: main.cpp
	$(CC) $(CXXFLAGS) $< -o $@

test: test.cpp
	$(CC) $(CXXFLAGS) $< -o $@
	./test

clean:
	rm -rf *.o
	rm -rf othello
	rm -rf test