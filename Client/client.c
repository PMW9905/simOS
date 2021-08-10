#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

void error(char* msg)
{
    perror(msg);
    exit(0);
}

void main(int argc, char* argv[])
{
    int sockfd, portno, ret, i;
    struct sockaddr_in serv_addr;
    struct hostent* server;
    char buffer[256];
    char filename[16];
    FILE* fp;

    if (argc < 3) {
        fprintf(stderr, "usage %s hostname port\n", argv[0]);
        exit(0);
    }
    portno = atoi(argv[2]);

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) error("ERROR opening socket");
    server = gethostbyname(argv[1]);
    if (server == NULL) error("ERROR, no such host");
    bzero((char*)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char*)server->h_addr,
        (char*)&serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(portno);

    if (connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0)
        error("ERROR connecting");


    int active = 1;
    while (active)
    {
        printf("(T = Terminate; s = Submit) \nEnter Command: \n");
        scanf("%s", &filename);
        if (filename[0] == 'T')
        {
            ret = write(sockfd, "q", 1);
            if (ret < 0) error("ERROR writing to socket");
            printf("I just sent quit message\n");
            printf("quitting...\n");
            active = 0;
        }
        else if (filename[0] == 's')
        {
            printf("Submission file: ");
            scanf(" %s", &filename);
            fp = fopen(filename, "r");
            if (fp == NULL)
            {
                printf("file not found");
                exit(1);
            }
            i = 0;

            //printf("Beginning file read\n");
            while (1)
            {
                buffer[i] = fgetc(fp);
                //printf("%c", buffer[i]);
                if (buffer[i] == EOF)
                {
                    buffer[i] = '\0';
                    break;
                }
                i++;
            }
            fclose(fp);
            //printf("Completed file read.\n");



            ret = write(sockfd, buffer, strlen(buffer));
            if (ret < 0) error("ERROR writing to socket");
            //printf("I just sent buffer");

            do
            {
                bzero(buffer, 256);
                ret = read(sockfd, buffer, 255);
                if (ret < 0) error("ERROR reading from socket");
                printf("%s\n", buffer);
            } while (buffer[2] != 'o' && buffer[0] != '\0');
        }
        else
            printf("Invalid input\n");
    }
    close(sockfd);
}
//printf("Please enter the message: ");
//scanf("%s", buffer);
//ret = write(sockfd, buffer, strlen(buffer));
//if (ret < 0) error("ERROR writing to socket");
//bzero(buffer, 256);
//ret = read(sockfd, buffer, 255);
//if (ret < 0) error("ERROR reading from socket");
//printf("%s\n", buffer);
