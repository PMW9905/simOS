#include "simos.h"
#include <string.h>
#include <signal.h>//signals

void one_admin_command(char *act);

void catch_sigint(int sig)
{
    memset(adminCommand,0,100);
    //printf("adminCommand before: %s\n",adminCommand);
    read(readAddr, adminCommand, 100);
    //printf("signal retrieved: %s\n",adminCommand);
    set_interrupt(adminCMDInterrupt);
}



void execute_process_iteratively ()
{  
    int i=0;
    signal(SIGINT, catch_sigint);
    while(systemActive && i<numR)
    {
        execute_process();
        i++;
    }
    char exit[1]={'T'};
    one_admin_command(exit);
}

void one_admin_command (char *act)
{ char fname[100];
  //printf("one_admin_command: %s\n",act);
  int i = 2;
  switch (act[0])
  { case 'T':  // Terminate simOS
      systemActive = 0; break;
    //case 's':  // submit a program: should not be admin's command
    //    for (i = 2; i < 100; i++)
    //        fname[i - 2] = act[i];
    //    submit_process(fname);
    //	fprintf(writePipe,"\n");
    //	fflush(writePipe);
    //    break;
    //case 'x':  // execute once, duplicate with y, but more convenient
      //execute_process (); break;
    //case 'y':  // multiple rounds of execution
      //execute_process_iteratively (); break;
    case 'q':  // dump ready queue and list of processes completed IO
      dump_ready_queue (writePipe); dump_endIO_list (writePipe); break;
    case 'r':   // dump the list of available PCBs
      dump_registers (writePipe); break;
    case 'p':   // dump the list of available PCBs
      dump_PCB_list (writePipe); break;
    case 'm':   // dump memory of each process
      dump_PCB_memory (writePipe); break;
    case 'f':   // dump memory frames and free frame list
      dump_memoryframe_info (writePipe); break;
    case 'n':   // dump the content of the entire memory
      dump_memory (writePipe); break;
    case 'e':   // dump events in clock.c
      dump_events (writePipe); break;
    case 't':   // dump terminal IO queue
      dump_termIO_queue (writePipe); break;
    case 'w':   // dump swap queue
      dump_swapQ (writePipe); break;
    default:   // can be used to yield to client submission input
      fprintf (writePipe, "Error: Incorrect command!!!\n");
  }
}

//void process_admin_commands ()
//{ 

    
    //char action[10];

  //while (systemActive)
  //{ fprintf (infF, "command> ");
  //  scanf ("%s", action);
  //  if (uiDebug) fprintf (bugF, "Command issued: %c\n", action[0]);
  //  // only first action character counts, discard remainder
  //  // character string is used to capture <ret>
  //  one_admin_command (action[0]);
  //}
//}

