#include "simos.h"

#include <pthread.h>
#include <semaphore.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>

//===============================================================
// The interface to interact with clients for program submission
// --------------------------
// Should change to create server socket to accept client connections
// -- Best is to use the select function to get client inputs
// Should change term.c to direct the terminal output to client terminal
//===============================================================


pthread_t submitThread;
int sockfd, newsockfd, clilen;
char buffer[256];
struct sockaddr_in serv_addr, cli_addr;
int ret;
fd_set active_fd_set;


int processCount = 1;
node_t *head = NULL;


void respond_to_client(int fd)
{
	char tempFileName[8] = { 0 };
	snprintf(tempFileName, 8, "%d.txt", processCount++);
	//printf("Filename: %s\n", tempFileName);
	bzero(buffer, 256);
	ret = read(fd, buffer, 255);
	if (ret < 0)
		error("ERROR reading from socket");
	if(buffer[0]=='\0' || buffer[0]=='q')
		FD_CLR(fd, &active_fd_set);
	else
	{
		//make file
		FILE* fp;
		fp = fopen(tempFileName, "w");
		if (fp == NULL)
			printf("unable to create file\n");
		else
			fputs(buffer, fp);
		fclose(fp);

		//add to queue
		cNode_t* cli;
		cli = malloc(sizeof(cNode_t));
		cli->fileName = tempFileName;
		cli->sockfd = fd;
		enqueue(*cli);

		set_interrupt(submitInterrupt);
		//remove(tempFileName); //temporary; reduces spam
	}
}

void accept_client()
{
	clilen = sizeof(cli_addr);
	newsockfd = accept(sockfd, (struct sockaddr*)&cli_addr, &clilen);
	if (newsockfd < 0)
		error("ERROR accepting");
	else
	{
		printf("Accepted client socket %d %d\n",newsockfd, (int)cli_addr.sin_port);
		FD_SET(newsockfd, &active_fd_set);
	}
}

void socket_select()
{
	int i;
	fd_set read_fd_set;
	FD_ZERO(&active_fd_set);
	FD_SET(sockfd, &active_fd_set);

	while (systemActive)
	{
		read_fd_set = active_fd_set;
		if(select(FD_SETSIZE,&read_fd_set,NULL,NULL,NULL)<0)
		{
			perror("select"); 
			exit(EXIT_FAILURE);
		}

		for(i=0;i<FD_SETSIZE;++i)
			if (FD_ISSET(i, &read_fd_set))
			{
				if (i == sockfd)
					accept_client();
				else
					respond_to_client(i);
			}
	}
}


void* thread_submit_socket()
{

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
		error("ERROR opening socket");

	bzero((char*)& serv_addr, sizeof(serv_addr));

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(port);

	if (bind(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0)
		error("ERROR binding");
	listen(sockfd, 5);

	socket_select();
	close(sockfd);
}

void initialize_submit_thread()
{
	int ret;
	ret = pthread_create(&submitThread, NULL, thread_submit_socket, NULL);
}


void program_submission ()
{ char fname[100];

  fprintf (infF, "Submission file: ");
  scanf ("%s", &fname);
  if (uiDebug) fprintf (bugF, "File name: %s has been submitted\n", fname);
  //submit_process (fname);
}


//queue def

void enqueue(cNode_t cli)
{
	node_t *new_node = malloc(sizeof(node_t));
	if(!new_node) return;
	new_node->payload = cli;
	new_node->next = head;
	head = new_node;
}

cNode_t* dequeue()
{
	node_t *current, *previous = NULL;
	cNode_t* cli = NULL;
	if(head==NULL)
		return NULL;
	current = head;
	while(current->next !=NULL)
	{
		previous = current;
		current = current->next;
	}
	cli = &(current->payload);
	if(previous)
		previous->next=NULL;
	else
		head = NULL;
	return cli;
}





