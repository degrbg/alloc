CXX_C = clang++
CXX_G = g++-11
CXXFLAGS = -std=c++17 -g -O0
SRC = main.cpp

all: main

main: $(SRC)
	$(CXX_C) $(CXXFLAGS) -o a.c $(SRC)
	$(CXX_G) $(CXXFLAGS) -o a.g $(SRC)

clean:
	rm -f -r -v a.c*
	rm -f -r -v a.g*

