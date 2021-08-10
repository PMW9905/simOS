#include "simos.h"

//=====================================
// system.c reads in system wide parameters from "config.sys"
// and calls the intialization functions in each module
//=====================================

void configure_system ()
{ FILE *fconfig;
  char str[60];

  fconfig = fopen ("config.sys", "r");
  fscanf (fconfig, "%d %d %d %s\n",
          &maxProcess, &cpuQuantum, &idleQuantum, str);
  fscanf (fconfig, "%d %d %s\n", &pageSize, &numFrames, str);
  fscanf (fconfig, "%d %d %d %s\n", &loadPpages, &maxPpages, &OSpages, str);
  fscanf (fconfig, "%d %d %d %d %s\n",
          &agescanPeriod, &instrTime, &termPrintTime, &diskRWtime, str);
  fscanf (fconfig, "%d %d %d %d %d %d %s\n",
          &cpuDebug, &memDebug, &termDebug, &swapDebug, &clockDebug, 
          &uiDebug, str);
  fclose (fconfig);

  bugF = fopen ("debug.tmp", "w");
  infF = stdout;
  writePipe = fdopen(writeAddr,"w");
  // bugF = stderr;
}

void initialize_system ()
{
  configure_system ();

  //========== initialize the data structures in the main thread
  initialize_timer ();
  initialize_cpu ();
  initialize_physical_memory ();  // 3 memory initialization
  initialize_mframe_manager ();
  initialize_agescan ();
  initialize_process_manager ();

  //========== start the other two threads
  start_terminal ();   // term.c
  start_swap_manager ();   // swap.c
  initialize_submit_thread(); //submit.c
}

void system_exit ()
{
  // wait for the other threads to clean up and terminate
  end_terminal ();
  end_swap_manager ();

  fclose (bugF);
}

void startSimOS(int n, int p, int r, int w) 
{ 
  char action[10];
  systemActive = 1;
  readAddr = r;
  writeAddr = w;
  numR = n;
  port = p;
  // swap and term threads have a while loop controlled by systemActive
  // admin's while loop is also controlled by systemActive
  // admin's T command sets systemActive to 0, stop all threads
  initialize_system ();
  execute_process_iteratively();
  fprintf (infF, "System exiting!!!\n");
  system_exit ();
}

