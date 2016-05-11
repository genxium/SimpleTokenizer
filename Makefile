CXX=g++
all: main.out

main.out:	main.o
	$(CXX) -o main.out main.o

main.o:	main.cpp
	$(CXX) -o main.o -c main.cpp
