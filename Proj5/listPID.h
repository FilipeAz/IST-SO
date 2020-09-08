

#ifndef LISTPID_H
#define LISTPID_H

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


/* lst_iitem - each element of the list points to the next element */
typedef struct lst_ppid* list_pid;

typedef struct lst_ppid {
   int pid;
   struct lst_ppid *next;
} lst_ppid_t;



list_pid lst_new_terminal();

int lst_destroy_terminal(list_pid list, int sig);

list_pid insert_new_terminal(list_pid list, int pid);

#endif
