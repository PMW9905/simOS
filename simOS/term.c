#include <pthread.h>
#include <semaphore.h>
#include "simos.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>

//=========================================================================
// Terminal manager is responsible for printing an output string to terminal,
//    which is simulated by a file "terminal.out"
// Terminal is a separate device from CPU/Mem, so it is a separate thread.
//    Some accesses interfere with the main thread and protection is needed
//    Use semaphores to make it thread safe under concurrent accesses
// When there is an output, the process has to be in eWait state and
//    We insert both pid and the output string to the terminal queue.
// Terminal manager manages the queue and process printing jobs in the queue
// After printing is done, we put the process back to the ready state,
//    which has to be done by the process manager.
//    Terminal only puts the process in endIO queue and set the interrupt.
//=========================================================================

// terminal output file name and file descriptor
// write terminal output to a file, to avoid messy printout
#define termFN "terminal.out"   
FILE *fterm;
sem_t insert_sem;
sem_t thread_sem;


void terminal_output (FILE *fterm, int pid, char *outstr)
{
    int ret;
    ret = write(PCB[pid]->sockfd, outstr, 255);
    if (ret < 0)
        error("ERROR writing to socket");
    if (outstr[2] == 'o')
    {
        remove(PCB[pid]->fileName);
    }

  //fprintf (fterm, "%s\n", outstr);
  //fflush (fterm);


  usleep (termPrintTime);  // simulate the delay for terminal output
}

//=========================================================================
// terminal queue 
// implemented as a list with head and tail pointers
//=========================================================================

typedef struct TermQnodeStruct
{ int pid, type;
  char *str;
  struct TermQnodeStruct *next;
} TermQnode;

TermQnode *termQhead = NULL;
TermQnode *termQtail = NULL;


// dump terminal queue is not called inside the terminal thread,
// only called by admin.c
void dump_termIO_queue (FILE *outf)
{ TermQnode *node;

  fprintf (outf, "******************** Term Queue Dump\n");
  node = termQhead;
  while (node != NULL)
  { fprintf (outf, "%d, %s\n", node->pid, node->str);
    node = node->next;
  }
  fprintf (outf, "\n");
}

// insert terminal queue is called by the main thread when
//    terminal output is needed (only in cpu.c, process.c)
void insert_termIO (pid, outstr, type)
int pid, type;
char *outstr;
{ TermQnode *node;
  sem_wait(&thread_sem);//starts as 1, so insert_termIO always first
  if (termDebug) fprintf (bugF, "Insert term queue %d %s\n", pid, outstr);
  node = (TermQnode *) malloc (sizeof (TermQnode));
  node->pid = pid;
  node->str = outstr;
  node->type = type;
  node->next = NULL;
  if (termQtail == NULL) // termQhead would be NULL also
    { termQtail = node; termQhead = node; }
  else // insert to tail
    { termQtail->next = node; termQtail = node; }
  if (termDebug) dump_termIO_queue (bugF);
  sem_post(&thread_sem);//first to prevent handler from breaking mutual exlusion
  sem_post(&insert_sem);//allows handler to run
}

// remove the termIO job from queue and call terminal_output for printing
// after printing, put the job to endIO list and set endIO interrupt
void handle_one_termIO ()
{ TermQnode *node;
  sem_wait(&insert_sem);//waits first for insert
  sem_wait(&thread_sem);
  if (termDebug) dump_termIO_queue (bugF);
  if (termQhead == NULL)
  { if (systemActive)
      fprintf (infF, "Error: No process in term queue!!!\n");
  }
  else 
  { node = termQhead;
    //printf("about to terminal_output\n");
    terminal_output (fterm, node->pid, node->str);

    if (node->type != exitProgIO)
    { // *** ADD CODE: What should be done after IO is done?
        insert_endIO_list(node->pid);
        set_interrupt(endIOinterrupt);
    } // if it is of exitProgIO type, then job done, just clean termIO queue

    if (termDebug)
      fprintf (bugF, "Remove term queue %d %s\n", node->pid, node->str);
    termQhead = node->next;
    if (termQhead == NULL) termQtail = NULL;
    free (node->str); free (node);
    if (termDebug) dump_termIO_queue (bugF);
  }
  sem_post(&thread_sem);//indicates that thread has finished
}


//=====================================================
// loop on handle_one_termIO to process the termIO requests
// This has to be a separate thread to loop for request handling
//=====================================================

void *termIO ()
{
  while (systemActive) handle_one_termIO ();
  if (termDebug) fprintf (bugF, "TermIO loop has ended\n");
}

// initializing the terminal device
// terminal thread is created here and this function is invoked by system.c
pthread_t termThread;

void start_terminal ()
{ int ret;

  fterm = fopen (termFN, "w");
  // *** ADD CODE: create terminal thread, init semaphores for synchronization
  sem_init(&insert_sem, 0, 0);
  sem_init(&thread_sem, 0, 1);
  ret = pthread_create(&termThread, NULL, termIO, NULL);
}

// cleaning up and exit the terminal thread, invoked by system.c
void end_terminal ()
{ int ret;

  fclose (fterm);
  // *** ADD CODE: thread and semaphore finishing actions
  sem_post(&insert_sem);
  ret = pthread_join(termThread, NULL);
  fprintf (infF, "TermIO thread has terminated %d\n", ret);
}


