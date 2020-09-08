main: main.o commandlinereader.o
	gcc -o main commandlinereader.o main.o

commandlinereader.o: commandlinereader.c commandlinereader.h
	gcc -Wall -g -c commandlinereader.c

main.o: main.c commandlinereader.h
	gcc -Wall -g -c main.c
