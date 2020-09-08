/*
 * list.c - implementation of the integer list functions 
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


void insert_new_process(list_t *list, int pid, time_t starttime){
	lst_iitem_t *item;
	
	item = (lst_iitem_t *) malloc (sizeof(lst_iitem_t));
	item->pid = pid;
	item->status = 0; 
	item->starttime = starttime;
	item->endtime = 0;
	item->next = list->first;
	list->first = item;
}


void update_terminated_process(list_t *list, int pid, time_t endtime, int status)
{
   lst_iitem_t *list_aux= list->first;
   while((list_aux!=NULL) && (list_aux->pid!=pid)){
   	    list_aux=list_aux->next;
   }
   if(list_aux == NULL){
   	perror("No Process with this pid");
   	exit(EXIT_FAILURE);
   }
   list_aux->endtime = endtime;
   list_aux->duration = difftime(endtime,list_aux->starttime);
   list_aux->status = status;
   }



void lst_print(list_t *list)
{
	lst_iitem_t *item;

	
	item = list->first;
	/* while(1){ */ /* use it only to demonstrate gdb potencial */
	while (item != NULL){
		if (WIFEXITED(item->status) == 1 && item->pid != -1){
			printf("pid: %d\tstatus: %d\ttime:%d\n", item->pid, WEXITSTATUS(item->status),item->duration);
		}
		item = item->next; 
	}
	printf("-- end of list.\n");
}

