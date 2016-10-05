#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <error.h>
#include <time.h>

#define LISTENQ 5
#define MYPORT  5000
#define MAXSIZE 1024

void SendMsg(int to_sock, char msg[]);
void RecvMsg(int from_sock, char msg[]);
void RemoveLine(char msg[]);

int main(int argc, char *argv[])
{
    int sockfd, connfd;
    char receive[MAXSIZE];
    char reply[MAXSIZE];
    struct sockaddr_in sockserv;
    struct sockaddr_in sockclient;

    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("socket error");
        exit(EXIT_FAILURE);
    }
    
    memset(&sockserv, 0, sizeof(sockserv));
    sockserv.sin_family = AF_INET;
    sockserv.sin_port = htons(MYPORT);
    sockserv.sin_addr.s_addr = htonl(INADDR_ANY);
    
    if(bind(sockfd, (struct sockaddr *)&sockserv, sizeof(sockserv)) < 0)
    {
        perror("bind error");
        exit(EXIT_FAILURE);
    }
    
    if(listen(sockfd, LISTENQ) < 0)
    {
        perror("listen error");
        exit(EXIT_FAILURE);
    }
    
    while(1)
    {
        int len = sizeof(sockclient);
        if((connfd = accept(sockfd, (struct sockaddr *)&sockclient, &len)) < 0)
        {
            perror("accept error");
            exit(EXIT_FAILURE);
        }
        printf("Server get connection from %s\n", inet_ntoa(sockclient.sin_addr));
        SendMsg(connfd, "Connected");
        
        memset(receive, 0, sizeof(receive));
        memset(reply, 0, sizeof(reply));

        RecvMsg(connfd, receive);   
        if(strncmp(receive, "begin chat", strlen(receive) + 1) == 0)
        {
            printf("Client begin chat\n"); 
            SendMsg(connfd, "Ok, chat begin\n");
            
            while(1)
            {
                memset(receive, 0, sizeof(receive));
                RecvMsg(connfd, receive);
                if(strncmp(receive, "quit chat", strlen(receive) + 1) == 0)
                {
                    printf("Client Logoff\n");
                    break;
                }
                
                printf("Client: %s\n", receive);
                printf("Me: ");
                fgets(reply, sizeof(reply), stdin);
                RemoveLine(reply);
                SendMsg(connfd, reply);
            }
    
            close(connfd);

        }

    }

    return 0;

}


void SendMsg(int to_sock, char msg[])
{
    int len = (strlen(msg) + 1);
    if(send(to_sock, msg, len, 0) < 0)
    {
        perror("send error");
        exit(EXIT_FAILURE);
    }
}

void RecvMsg(int from_sock, char msg[])
{
    int byte_num;
    memset(msg, 0, MAXSIZE);
    byte_num = recv(from_sock, msg, MAXSIZE, 0);
    msg[byte_num] = '\0';
}        

void RemoveLine(char msg[])
{
    int i = 0;
    while(msg[i] != '\n')
        i++;
    msg[i] = '\0';
}
