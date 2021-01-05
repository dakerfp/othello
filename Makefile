
CC=g++
CXXFLAGS=-std=c++20 -O3 -Wall
VERSION=`git rev-parse --short HEAD`

all: test othello benchmark

othello: main.cpp
	$(CC) $(CXXFLAGS) -DVERSION=\"$(VERSION)\" $< -o $@

test: test.cpp
	$(CC) $(CXXFLAGS) $< -o $@
	./test || rm -rf test

benchmark: benchmark.cpp
	$(CC) $(CXXFLAGS) $< -o $@

run_benchmark: benchmark
	./benchmark 1000

perf: perf-kernel.svg

perf-report: benchmark
	./benchmark 10000 &
	sudo perf record -F 99 -p `pgrep benchmark` -g -- sleep 60
	pgrep benchmark | xargs kill -9

out.perf-folded: perf-report
	sudo perf script | FlameGraph/stackcollapse-perf.pl > $@

perf-kernel.svg: out.perf-folded
	FlameGraph/flamegraph.pl $< > $@

clean-perf:
	rm -rf perf-kernel.svg
	rm -rf out.perf-folded

clean: clean-perf
	rm -rf *.o
	rm -rf othello
	rm -rf test
	rm -rf benchmark
