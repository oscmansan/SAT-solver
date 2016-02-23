CC = g++
CFLAGS = -std=c++11 -O3 -Wall 

all: oscarSAT

oscarSAT: oscarSAT.cpp
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f oscarSAT