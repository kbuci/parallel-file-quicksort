all: csvTest.cpp
	g++ -std=c++11 -pthread csvLine.hpp quicksorts-revised.hpp csvTest.cpp -o quicksorter
