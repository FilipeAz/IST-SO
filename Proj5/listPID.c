#include "listPID.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <time.h>
#include <sys/types.h>


list_pid lst_new_terminal()
{
   list_pid list;
   list = (list_pid) malloc(sizeof(lst_ppid_t));
   return list;
}


int lst_destroy_terminal(list_pid list,int sig )
{
  int pid;
  list_pid item, nextitem;

  item = list;
  while (item != NULL){
    nextitem = item->next;
    pid=(item->pid);
    kill(pid,sig);
    free(item);
    item = nextitem;
  }
  free(list);
  return 0;
}


list_pid insert_new_terminal(list_pid list, int pid)
{
  list_pid item;

  item = (list_pid) malloc (sizeof(lst_ppid_t));
  item->pid = pid;
  item->next = list;
  list = item;
  return list;
}
