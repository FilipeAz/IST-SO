par-shell: par-shell.o commandlinereader.o list.o fibonacci 
	gcc -o par-shell commandlinereader.o list.o par-shell.o -lpthread

commandlinereader.o: commandlinereader.c commandlinereader.h
	gcc -Wall -g -c commandlinereader.c

par-shell.o: par-shell.c commandlinereader.h list.h
	gcc -Wall -g -c par-shell.c
	
fibonacci: fibonacci.c
	gcc -Wall -g -o fibonacci fibonacci.c

list.o: list.c list.h
	gcc -Wall -g -c list.c

clean:
	rm -f *.o par-shell fibonacci

