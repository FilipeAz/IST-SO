#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "commandlinereader.h"

#define EXIT_COMMAND   "exit"
#define MAXARGS        7
#define BUFFER_SIZE    100

int main(int argc, char **argv) {

	char buffer[BUFFER_SIZE];
	char buf[BUFFER_SIZE];
	char pid[BUFFER_SIZE];
	char *pipeAux = (char*) malloc(sizeof(char) * BUFFER_SIZE); 
	int fout, numargs, fin, i;
	char *args[MAXARGS]; 
	char command[BUFFER_SIZE];
	int total_time;
	int num_children;

	
	
	if ((fin = open(argv[1], O_WRONLY, S_IRUSR | S_IWUSR)) < 0){
		exit(-1);
	}
	sprintf(command, "pid %d\n", getpid());
	if(write(fin, command,BUFFER_SIZE)  == -1){
				perror("write");
				exit (-1); 
	}
	memset(command,0,BUFFER_SIZE);

	while(1){
		numargs = readLineArguments(args, MAXARGS, buffer, BUFFER_SIZE);
		if(numargs == 0){
			continue;
		}
		strcpy(command,args[0]);
		for(i=1;i<numargs;i++){
			strcat(command," ");
			strcat(command ,args[i]);
		}
		if(strcmp(args[0], EXIT_COMMAND) == 0){
			sprintf(pid, " %d", getpid());
			strcat(command,pid);
			if(write(fin, command,BUFFER_SIZE)  == -1){
				perror("write");
				exit (-1); 
			}
			printf("O par-shell-terminal vai terminar\n");
			break;
		}
		if(strcmp(args[0], "exit-global") == 0){
			strcat(command,"\n");
			if(write(fin, command,BUFFER_SIZE)  == -1){
				perror("write");
				exit (-1); 
			}
			break;
		}
		if (strcmp(args[0], "stats") == 0) {
			sprintf(pipeAux, "/tmp/par-shell-receiver-%d", getpid());
			unlink(pipeAux);
			if (mkfifo(pipeAux, 0666) == -1) {
        		exit (-1);
        	}
        
			strcat(command," ");
			strcat(command,pipeAux);
			strcat(command,"\n");
			if(write(fin, command,BUFFER_SIZE)  == -1){
				perror("write");
				exit (-1); 
			}
			memset(command,0,BUFFER_SIZE);
			if ((fout = open(pipeAux,O_RDONLY, S_IRUSR | S_IWUSR)) <0) 
				exit (-1);
			if(read(fout, buf, BUFFER_SIZE) == -1){
				perror("write");
				exit(-1);
			}
			close(fout);
			unlink(pipeAux);
			sscanf(buf,"%d %d\n",&total_time,&num_children);
			printf("O tempo total é %d; numero de processos em execucao é %d\n",total_time,num_children);
   		}

		else{
			strcat(command,"\n");
			if(write(fin, command, strlen(command)) == -1){
				perror("write");
				exit(-1);
			}
			memset(command,0,BUFFER_SIZE);

		}
	
	}
	close(fin);
	return 0;

}
