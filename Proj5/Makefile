# Makefile for par-shell, version 1
# Sistemas Operativos, DEI/IST/ULisboa 2015-16

GROUP_NUM=XX
EXERCISE_NUM=Y
ARCHIVE_NAME=G$(GROUP_NUM)_E$(EXERCISE_NUM).zip
CFLAGS=-g -Wall -pedantic -std=c99
CFLAGS=-g -Wall -pedantic 

all: par-shell fibonacci par-shell-terminal

par-shell: par-shell.o commandlinereader.o list.o listPID.o
	gcc -o par-shell par-shell.o commandlinereader.o list.o  listPID.o -pthread

par-shell.o: par-shell.c commandlinereader.h
	gcc $(CFLAGS) -c par-shell.c
	
par-shell-terminal.o: par-shell-terminal.c commandlinereader.h
	gcc $(CFLAGS) -c par-shell-terminal.c
	
par-shell-terminal: par-shell-terminal.o commandlinereader.o
	gcc $(CFLAGS) -o par-shell-terminal par-shell-terminal.o commandlinereader.o 

commandlinereader.o: commandlinereader.c commandlinereader.h
	gcc $(CFLAGS) -c commandlinereader.c

list.o: list.c list.h
	gcc $(CFLAGS) -c list.c
	
listPID.o: listPID.c listPID.h
	gcc $(CFLAGS) -c listPID.c

fibonacci: fibonacci.c
	gcc $(CFLAGS) -o fibonacci fibonacci.c


clean:
	rm -f *.o par-shell fibonacci par-shell-terminal  core $(ARCHIVE_NAME)

dist:
	zip $(ARCHIVE_NAME) *.c *.h Makefile
