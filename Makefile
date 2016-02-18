all:
	g++ -std=c++11 -O3 -o oscarSAT oscarSAT.cpp

clean:
	rm oscarSAT