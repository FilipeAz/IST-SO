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

/*----------Incializacao das variaveis de condicao e trincos----------------------------*/
pthread_mutex_t data_ctrl;
pthread_cond_t  max_process;
pthread_cond_t  no_process;
 


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
void cond_w(pthread_cond_t *cond) {
  if(pthread_cond_wait(cond,&data_ctrl) != 0)
  {
    fprintf(stderr, "Error in pthread_cond_wait()\n");
    exit(EXIT_FAILURE);
  }
}

/* 
+-----------------------------------------------------------------------*/
void cond_s(pthread_cond_t *cond) {
  if(pthread_cond_signal(cond) != 0)
  {
    fprintf(stderr, "Error in pthread_cond_post()\n");
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
		/*Se nao houver processos filho a correr a tarefa monitora espera*/
        while(num_children == 0 && flag_exit == 0){
          cond_w(&no_process);

        }  
          mutex_unlock();
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
    mutex_lock();
    num_children --;
    cond_s(&max_process); /*Ja se pode executar mais um processo*/
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
  if(pthread_cond_init(&max_process,NULL)!= 0){
    fprintf(stderr, "Error creating pthread.\n");
    exit(EXIT_FAILURE);
  }
  /* Incializa e verifica se o semaforo foi corretamente inicializado */
  if(pthread_cond_init(&no_process,NULL)!= 0) {
    fprintf(stderr, "Error creating pthread.\n");
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

    mutex_lock();
	/*Se ja houver o numero maximo de processos filho a correr esta thread espera que um deles acabe antes de lancar um novo processo*/
    while(num_children >= maxp){ 
      cond_w(&max_process); 
    }
 

    start_time = time(NULL);
    pid = fork();   /*cria um processo filho*/
    
    /*caso tenha ocorrido erro na criacao do processo filho
    o programa E terminado*/ 
    if (pid == -1) {   
      perror("Failed to create new process.");
      exit(EXIT_FAILURE);
    }

    if (pid > 0) {  /* parent */
      num_children ++;
      insert_new_process(proc_data, pid, start_time);
	  /*Ha pelo menos um processo filho a correr pelo que a tarefa monitora se deve desbloquear*/
      cond_s(&no_process);
      mutex_unlock();
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
  cond_s(&no_process);
  mutex_unlock();

  
  /* espera que a tarefa monitora termine */
  if(pthread_join(tid, NULL) != 0) {
    fprintf(stderr, "Error joining thread.\n");
    exit(EXIT_FAILURE);
  }
  /*Imprime a lista onde esta guardada a informacao para o ficheiro log.txt*/
  lst_print(proc_data);

  /* destroi as variaveis de condicao, o mutex e a lista simplesmente ligada e termina o programa */
  pthread_cond_destroy(&max_process);
  pthread_cond_destroy(&no_process);
  pthread_mutex_destroy(&data_ctrl);
  lst_destroy(proc_data);
  return 0; /* ok */
}

