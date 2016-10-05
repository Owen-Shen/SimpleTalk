#include <stdio.h>
#include <error.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define DESTPORT 5000
#define MAXSIZE  1024

void SendMsg(int to_sockfd, char msg[]);
void RecvMsg(int from_sockfd,char msg[]);
void RemoveLine(char msg[]);

int main(int argc, char *argv[])
{
    int sockfd;
    struct sockaddr_in sockserv;
    char receive[MAXSIZE];
    char reply[MAXSIZE];
    
    if(argc < 2)
    {
        printf("Usage: a.out <IPaddress>\n");
        exit(EXIT_FAILURE);
    }

    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("socket error");
        exit(EXIT_FAILURE);
    }
    
    memset(&sockserv, 0, sizeof(sockserv));    
    sockserv.sin_family = AF_INET;
    sockserv.sin_port = htons(DESTPORT);
    if(inet_pton(AF_INET, argv[1], &sockserv.sin_addr) <= 0)
    {
        fprintf(stderr, "inet_pton error for %s", argv[1]);
        exit(EXIT_FAILURE);
    }
    
    if(connect(sockfd, (struct sockaddr *)&sockserv, sizeof(sockserv)) < 0)
    {
        perror("connect error");
        exit(EXIT_FAILURE);
    }
    
    else
    {
        memset(receive, 0, sizeof(receive));
        RecvMsg(sockfd, receive);
        if(strncmp(receive, "Connected", strlen(receive) + 1) == 0)
        {
            printf("%s\n", receive);
            while(1)
            {
                memset(reply, 0, sizeof(reply));
                printf("Me: ");
                fgets(reply, sizeof(reply), stdin);
                RemoveLine(reply);
                SendMsg(sockfd, reply);
                if(strncmp(reply, "quit chat", strlen(reply) + 1) == 0)
                {
                    printf("End chat\n");
                    break;
                }
                memset(receive, 0, sizeof(receive));
                RecvMsg(sockfd, receive);
                printf("Server: %s\n", receive);

            }

        }

    }

    return 0;

}

void SendMsg(int to_sockfd, char *msg)
{
    int len = strlen(msg) + 1;
    if(send(to_sockfd, msg, len, 0) < 0)
    {
        perror("send error");
        exit(EXIT_FAILURE);
    }
}

void RecvMsg(int from_sockfd, char msg[])
{
    int type_num;
    memset(msg, 0, MAXSIZE);
    type_num = recv(from_sockfd, msg, MAXSIZE, 0);
    msg[type_num] = '\0';
}

void RemoveLine(char msg[])
{
    int i = 0;
    while(msg[i] != '\n')
        i++;
    msg[i] = '\0';
}        
