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
  int totaltime = 0, max=0, maxpid=0;
  lst_iitem_t *item;
  item = list->first;
  printf("\nList of processes:\n");
  while (item != NULL){
    if(WIFEXITED(item->status)){
	printf("pid: %d exited normally; status=%d.", item->pid, WEXITSTATUS(item->status));}
    else{
	printf("pid: %d terminated without calling exit.", item->pid);}
    printf(" Execution time: %d s\n", (int)(item->endtime - item->starttime));
	totaltime += (item->endtime - item->starttime);
	if(max<(item->endtime - item->starttime)){
		max = item->endtime - item->starttime;  
		maxpid = item->pid;
	  }
    item = item->next;
  
  }
  printf("tempo total: %d\n",totaltime);
  printf("O pid do processo de maior duração foi: %d que demorou: %d\n",maxpid,max);
  printf("End of list.\n");

}
