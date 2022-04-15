CXX = clang++
CXXFLAGS = -std=c++17 -g -O0
SRC = main.cpp

all: main

main: $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC)

