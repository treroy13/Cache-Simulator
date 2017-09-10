#specify target
all: cache-sim

cache-sim: cache.o
	    g++ -std=c++11 cache.o -o cache-sim

cache.o: cache.cpp
	      g++ -std=c++11 -c cache.cpp

clean:
	rm -f *.o cache-sim
