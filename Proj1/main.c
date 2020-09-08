#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include "commandlinereader.h"
#define N_ARGS 7
#define N_CHILD_PROC 1000000

int main(int argc,char** argv){
	int c=0,x,pid,status;
	long int c_pid[N_CHILD_PROC][2];
	char *args[N_ARGS];
	readLineArguments(args,N_ARGS);
	while(strcmp(args[0],"exit")){
		pid = fork();
		if (pid == 0){
			execv(args[0],args);
			perror("Nao foi possivel executar o programa");
			exit(-1);
		}
		else if(pid < 0){
			perror("Nao foi possivel criar shell paralela");
			continue;
		}
		else{
			c++;
		}
		readLineArguments(args,N_ARGS);
	}
	for(x=0;x<c;x++){
		c_pid[x][0]=wait(&status);
		c_pid[x][1]=status;
	}
	for(x=0;x<c;x++){
		printf("PID %ld inteiro %ld\n",c_pid[x][0],c_pid[x][1]);
	}
	exit(0);
}