/*Grupo 58:
  Filipe Azevedo n 82468;
  Pedro Santos   n 82507:
  Martim Zanatti n 82517;
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <semaphore.h> 
#include "commandlinereader.h"
#include "list.h"

#define EXIT_COMMAND "exit"

#define MAXARGS        7
#define BUFFER_SIZE  100
#define MAXPAR 4

/*-------------------Variaveis Globais:---------------------------------*/
int num_children = 0;
int flag_exit = 0; /* do not exit */

/*----------Incializacao da estrutura partilhada (list)------------------
E criada uma lista simplesmente ligada, onde vao ser guardadas as informacoes
dos processos filhos.
*/
list_t *proc_data;

/*----------Incializacao dos semaforos e trincos----------------------------*/
pthread_mutex_t data_ctrl;
sem_t sem_Process;
sem_t sem_Espera;



/* 
+-----------------------------------------------------------------------*/
void mutex_lock(void) {
  if(pthread_mutex_lock(&data_ctrl) != 0)
  {
    fprintf(stderr, "Error in pthread_mutex_lock()\n");
    exit(EXIT_FAILURE);
  }
}


/* 
+-----------------------------------------------------------------------*/
void mutex_unlock(void) {
  if(pthread_mutex_unlock(&data_ctrl) != 0)
  {
    fprintf(stderr, "Error in pthread_mutex_unlock()\n");
    exit(EXIT_FAILURE);
  }
}

/* 
+-----------------------------------------------------------------------*/
void sem_w(sem_t *sem) {
  if(sem_wait(sem) != 0)
  {
    fprintf(stderr, "Error in sem_wait()\n");
    exit(EXIT_FAILURE);
  }
}

/* 
+-----------------------------------------------------------------------*/
void sem_p(sem_t *sem) {
  if(sem_post(sem) != 0)
  {
    fprintf(stderr, "Error in sem_post()\n");
    exit(EXIT_FAILURE);
  }
}


/* 
-------------------------------------------------------------------------
tarefa_monitora: Recebe um ponteiro para o primeiro elemento da lista.
Esta funcao, caso existam processos filhos em execucao, esta constantemente
a esperar pela sua terminacao, medindo o tempo de e guardando-o. 
+-----------------------------------------------------------------------*/
void *tarefa_monitora(void *arg_ptr) {
  int status, childpid;
  time_t end_time;

  printf(" *** Tarefa monitora activa.\n");

  while(1) {
    /*Verifica se ja existem processos filhos em execucao e, caso a flag_exit
      seja igual a 1, a tarefa termina e imprime uma mensagem.*/
    mutex_lock();
    if(num_children == 0) {
      if(flag_exit == 1) {
	       mutex_unlock();
	       printf(" *** Tarefa monitora terminou.\n");
	       pthread_exit(NULL);
      }
      else {
        mutex_unlock();
        sem_w(&sem_Espera);
	     /* printf(" *** No children running.\n"); */
	     continue;
      }
    }
    mutex_unlock();

    /* espera que os processos filhos terminem */
    childpid = wait(&status);
    if (childpid == -1) {
      perror("Error waiting for child");
      exit(EXIT_FAILURE);
    }
    /*Atualiza a informacao dos processos terminados*/
    end_time = time(NULL);
    sem_p(&sem_Process);
    mutex_lock();
    num_children --;
    update_terminated_process(proc_data, childpid, end_time, status);
    mutex_unlock();
  }
}


/* 
+-----------------------------------------------------------------------*/
int main (int argc, char** argv) {
  pthread_t tid;
  char buffer[BUFFER_SIZE];
  int numargs;
  char *args[MAXARGS];
  time_t start_time;
  int pid;
  int maxp;

  /*Caso receba um numero como argumento esse passara a ser o numero maximo
    de processos filhos permitidos*/
  if (argc==2){ 
  maxp=atoi(argv[1]);  }

  /*Caso contrario, o  numero maximo de processos E o predefinido, MAXPAR*/
  else{ maxp=MAXPAR;}

  /* Incializa e verifica se o semaforo foi corretamente inicializado */
  if(sem_init(&sem_Process, 0, maxp)!= 0) {
    fprintf(stderr, "Error creating semaphore.\n");
    exit(EXIT_FAILURE);
  }
  /* Incializa e verifica se o semaforo foi corretamente inicializado */
  if(sem_init(&sem_Espera, 0, 0)!= 0) {
    fprintf(stderr, "Error creating semaphore.\n");
    exit(EXIT_FAILURE);
  } 

  /* criar estrutura de dados de monitorizacao */
  proc_data = lst_new();

  /* Incializa e verifica se o mutex foi corretamente inicializada */ 
  if(pthread_mutex_init(&data_ctrl, NULL) != 0) {
    fprintf(stderr, "Error creating mutex.\n");
    exit(EXIT_FAILURE);
  }

  /* Incializa e verifica se a tarefa (thread) foi corretamente inicializada */
  if (pthread_create(&tid, NULL, tarefa_monitora, NULL) != 0) {
    fprintf(stderr, "Error creating thread.\n");
    exit(EXIT_FAILURE);
  }

  printf("Insert your command: ");
  while(1) {
     numargs = readLineArguments(args, MAXARGS, buffer, BUFFER_SIZE);

    if (numargs < 0) {
      printf("Error in readLineArguments()\n");
      continue;
    }
    if (numargs == 0) /* empty line; read another one */
      continue;

    /* Verifica se chegou o comando exit, caso sim imprime uma mensagem de fim, 
    termina o ciclo while e o programa vai terminar ordeiramente*/
    if (strcmp(args[0], EXIT_COMMAND) == 0) {
      printf("Ending...\n");
      break;
    }
    /* process a command */

    sem_w(&sem_Process);
    start_time = time(NULL);
    pid = fork();   /*cria um processo filho*/
    
    /*caso tenha ocorrido erro na criacao do processo filho
    o programa E terminado*/ 
    if (pid == -1) {   
      perror("Failed to create new process.");
      exit(EXIT_FAILURE);
    }

    if (pid > 0) {  /* parent */
      mutex_lock();
      num_children ++;
      insert_new_process(proc_data, pid, start_time);
      mutex_unlock();
      sem_p(&sem_Espera);
    }
    else if (pid == 0) {  /* child */
      if (execv(args[0], args) == -1) {
	perror("Could not run child program. Child will exit.");
	exit(EXIT_FAILURE);
      }
    }
  }

  mutex_lock();
  flag_exit = 1; /* Pede a tarefa monitora para terminar, atraves da flag_exit*/
  mutex_unlock();
  sem_p(&sem_Espera);
  
  /* espera que a tarefa monitora termine */
  if(pthread_join(tid, NULL) != 0) {
    fprintf(stderr, "Error joining thread.\n");
    exit(EXIT_FAILURE);
  }

  lst_print(proc_data);

  /* destroi os semaforos, o mutex e a lista simplesmente ligada e termina o programa */
  sem_destroy(&sem_Espera);
  sem_destroy(&sem_Process);
  pthread_mutex_destroy(&data_ctrl);
  lst_destroy(proc_data);
  return 0; /* ok */
}

