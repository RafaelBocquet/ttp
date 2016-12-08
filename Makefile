all: main.cpp header.h
	g++ -Wall -Wextra -std=c++1z main.cpp -o main -g
opt: main.cpp header.h
	g++ -Wall -Wextra -std=c++1z -Ofast -march=native main.cpp -o main
