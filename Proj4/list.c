/*
 * list.c - implementation of a linked list
 */


#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "list.h"



list_t* lst_new()
{
   list_t *list;
   list = (list_t*) malloc(sizeof(list_t));
   list->first = NULL;
   return list;
}


void lst_destroy(list_t *list)
{
  struct lst_iitem *item, *nextitem;

  item = list->first;
  while (item != NULL){
    nextitem = item->next;
    free(item);
    item = nextitem;
  }
  free(list);
}


void insert_new_process(list_t *list, int pid, time_t starttime)
{
  lst_iitem_t *item;

  item = (lst_iitem_t *) malloc (sizeof(lst_iitem_t));
  item->pid = pid;
  item->starttime = starttime;
  item->endtime = 0;
  item->status = 0;
  item->next = list->first;
  list->first = item;
}


void update_terminated_process(list_t *list, int pid, time_t endtime, int status)
{
  lst_iitem_t *item;

  item = list->first;
  while(item != NULL) {
    if(item->pid == pid)
    {
      item->endtime = endtime;
      item->status = status;
      return;
    }
    item = item->next;
  }
  printf("list.c: update_terminated_process() error: pid %d not in list.\n", pid);
}


void lst_print(list_t *list)
{
  lst_iitem_t *item;
  FILE * fp;
  char *iter = (char*) malloc(sizeof(char) * 40); /*Guarda-se aqui a linha com a iteracao anterior*/
  char *total_time = (char*) malloc(sizeof(char) * 40); /*Guarda-se aqui o tempo total de execucao da par-shell*/
  int total_exe_time = 0, iteracao = 0, contador = 0; /*Contador necessario para verificar se log.txt esta vazio*/


  item = list->first;
  fp = fopen("log.txt","a+");
  while(fgets(iter,1000,fp) != NULL){
	/* Esta linha nao interessa portanto podemos usar temporariamente o mesmo local onde se vai guardar a linha com o tempo total de execucao*/
    fgets(total_time,1000,fp);
    fgets(total_time,1000,fp);
	contador++;
  } 
  /*Se log.txt ja continha informacao entao guarda-se a ultima iteracao e o tempo total de execucao*/
  if(contador != 0){
    sscanf(iter,"%*s %d",&iteracao);
    sscanf(total_time,"%*s %*s %*s %d %*s",&total_exe_time);
    iteracao++;
  }

  /*Escrita da informacao dos processos filho em log.txt*/
  while (item != NULL){
    fprintf(fp,"iteracao %d\n",iteracao++);
    if(WIFEXITED(item->status)){
      fprintf(fp,"pid: %d" , item->pid);
    }  
    else{
      fprintf(fp,"pid: %d terminated without calling exit.", item->pid);
    }  
    fprintf(fp," Execution time: %d s\n", (int)(item->endtime - item->starttime));
    total_exe_time += (int) (item->endtime - item->starttime);
    fprintf(fp,"total execution time: %d s\n", total_exe_time);
    item = item->next;
  }
  fflush(fp);
  fclose(fp);
  printf("End of list.\n");
  free(iter);
  free(total_time);
}

