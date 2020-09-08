/*Grupo 58:
Filipe Azevedo nº82468
Pedro Santos   nº82507
Martim Zanatti  nº82517
*/

/*
// Sistemas Operativos, DEI/IST/ULisboa 2015-16
*/

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include "list.h"
#include "commandlinereader.h"

#define EXIT_COMMAND "exit"
#define MAXARGS 7 /* Comando + 5 argumentos opcionais + espaco para NULL */
#define STATUS_INFO_LINE_SIZE 50
#define BUFFER_SIZE 100

/*INIALIZACAO DAS VARIAVEIS GLOBAIS: */
int numchildren = 0, flag = 0; /* numchildren:guarda o numero de processos filhos em execucao */
pthread_mutex_t trinco;       

void *Monitora(void *list);

int main (int argc, char** argv) {

  pthread_mutex_init(&trinco, NULL); 
 
  pthread_t threadMo;
  
  char *args[MAXARGS];
  char buffer[BUFFER_SIZE];
  
  /* Cria uma lista simplesmente ligada, onde vao ser guardadas as informacoes
  dos processos filhos */
  list_t *list = lst_new();

  /* Incializa e verifica se a tarefa Monitora foi corretamente inicializada */ 
  if(pthread_create (&threadMo, 0,Monitora,(void*)list)){
  	perror("Failed to create a thread.");
  	exit(EXIT_FAILURE);
  }
  
  printf("Insert your commands:\n");
    
  while (1) {
    int numargs;
    
    numargs = readLineArguments(args, MAXARGS, buffer, BUFFER_SIZE);
    
    /* Verifica se chegou ao EOF (end of file) do stdin ou se chegou a ordem "exit". 
    Em ambos os casos, termina ordeiramente. Inicialmente ative a flag, que vai informar a tarefa monitora
    para que quando nao hajam mais processos filhos a correr esta faca exit.  
    Aguarda que a tarefa monitora termine e imprime a informacao dos processos filhos criados */
    if (numargs < 0 ||
	(numargs > 0 && (strcmp(args[0], EXIT_COMMAND) == 0))) {
      flag = 1;
      pthread_join(threadMo,NULL);
      pthread_mutex_destroy(&trinco);
      lst_print(list);
      lst_destroy(list);
      exit(0);
    }
      
    /* Caso tenha havido argumentos e nao seja "exit", lancamos processo filho */
    else if (numargs > 0) {
      
      int pid = fork();
      time_t starttime = time(NULL);

      if (pid == -1) {
	     perror("Failed to create new process.");
	     exit(EXIT_FAILURE);
      }
      
      if (pid > 0) { 	  /* Codigo do processo pai:
      Guarda a informacao do novo processo filho, nomeadamente o pid 
      e o tempo de incio */  
        pthread_mutex_lock(&trinco);
      	insert_new_process(list,pid,starttime);
		    numchildren++;
        pthread_mutex_unlock(&trinco);
		    continue;
      }
      else { /* Codigo do processo filho */
	       if (execv(args[0], args) < 0) {
	         perror("Could not run child program. Child will exit.");
	         exit(EXIT_FAILURE);
	       }
      }
    }
  }
}
 
/* Funcao Monitora: recebe um ponteiro para o primeiro elemnto da list,
esta funcao espera pela terminacao dos processos filho. Para cada processo filho
terminado mede o tempo e guarda-o */
void *Monitora(void *list){
	int childpid,status;
	time_t endtime;

	while(1){
    /* Verifica se nao existem processos filhos em execucao, caso nao existam e a flag
     estiver a zero, a tarefa e terminada, se nao a funcao faz sleep durante um sugundo 
     e volta a verificar se existem processos filhos ativos */
  	if (numchildren == 0) {
      if(flag==1)
        pthread_exit(NULL);
		sleep(1);
    }
    /* Espera pela terminacao de cada filho */
    else {
		childpid = wait(&status);
        endtime = time(NULL);
        if(childpid < 0) {
			perror("Error waiting for child.");
			exit (EXIT_FAILURE);
		} 
		/* Atualiza a informacao dos processos terminados */
		pthread_mutex_lock(&trinco);
		update_terminated_process(list,childpid,endtime,status);
	    numchildren--;
		pthread_mutex_unlock(&trinco);
	}
  }
  return NULL;
}