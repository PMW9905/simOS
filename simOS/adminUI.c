#include "simos.h" 
#include <string.h>
#include <stdio.h> //io
#include <unistd.h> //for fork
#include <sys/types.h> //for pid_t
#include <signal.h> //signals

int main(int argc, char* argv[])
{
	int num;//numR stored in argv[1]
	int prt;
	int sP[2]; //send pipe; pipe used to send information to simos.
	int rP[2]; //recieve pipe; pipe used to recieve info from simos.
	if (argc != 3)
	{
		printf("Invalid number of arguments. Terminating process.");
		return 1;
	}
	if(pipe(sP)<0 || pipe(rP)<0)
	{
		printf("Pipe failure. Terminating process.");
		return 1;
	}

	sscanf(argv[1], "%d", &num);
	sscanf(argv[2], "%d", &prt);

	pid_t pid = fork();

	if (pid == 0)//child
	{
		close(sP[1]);
		close(rP[0]);
		startSimOS(num,prt,sP[0],rP[1]);
		close(sP[0]);
		close(rP[1]);
		/*run simos funciton that starts stuff up. 
		Paramaters: numR, read, write.
		Should:
		- initialize system 
	    - run execute_process numR times.
		- call system_exit at the end.
		*/
	}
	else //parent
	{
		FILE *parentRead=fdopen(rP[0],"r");
		char action[100]="";
		char fileName[100]="";
		char pOut[1000]="";
		int i=2;
		usleep(100000);
		close(sP[0]);
		close(rP[1]);
		while (1)
		{
			printf("command> ");
			scanf("%s", &action);	
			//if (action[0] == 's')
			//{
			//	printf("Submission file: ");
			//	scanf(" %s", &fileName);
			//	for(i=2;i<100;i++)
			//		action[i]=fileName[i-2];
			//}
			//printf("fileName: %c\n",fileName[0]);
			//printf("action: %c\n",action[0]);
			kill(pid, SIGINT);
			//printf("Parent just killed and is writing.\n");
			write(sP[1], action, 100);
			//printf("Parent just wrote\n");
			//wait for response from child
			memset(pOut,0,1000);
			//memset(action,0,100);
			//memset(fileName,0,100);
			read(rP[0], pOut, 1000);
			//fscanf(parentRead,"%s",pOut);
			//printf("Parent just read\n");
			printf("%s",pOut);
			//printf("Parent just printed\n");
			//read pipe (actually, might be able to dupe pipe to stdout!)
			//write pipe contents to screen
			
			if(action[0]=='T')
			{
				fflush(parentRead);
				usleep(50000);
				close(sP[1]);
				close(rP[0]);
				return 0;
			}
			for(i=0;i<100;i++){action[i]='\0';}
			for(i=0;i<100;i++){fileName[i]='\0';}
		}

		/*
		1) read admin command
		2) send signal to simos
		3) send via sP (send pipe)
		4) wait for response via parent read pipe
		5) print out response
		*/
	}
	return 0;


}
