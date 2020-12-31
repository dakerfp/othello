
CC=g++


othello: main.cpp
	$(CC) $< -o $@

test: test.cpp
	$(CC) $< -o $@
	./test

clean:
	rm -rf *.o
	rm -rf othello
	rm -rf test