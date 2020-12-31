
CC=g++


othello: main.cpp
	$(CC) $< -o $@

clean:
	rm -rf *.o
	rm -rf othello