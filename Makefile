CXX=gcc
all: main.out
preprocess: main.i

main.out:	main.o
	$(CXX) -o main.out main.o

main.o:	main.c
	$(CXX) -o main.o -c main.c

main.i: main.c
	$(CXX) -o main.i -E main.c
