// Jason Ryan
// ECE:3540
// Project
// Intermediate Server

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define SIZE 1024
#define HOST_PORT 15020

void err(char *msg)
{
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[]) {
    int sockfd, clientSockfd, portno, clientLen, optval, n, serverLen;
    struct sockaddr_in servAddr;
    struct sockaddr_in clientAddr;
    struct sockaddr_in hostAddr;
    struct hostent *hostPtr;
    struct hostent *serverPtr;
    char * token[SIZE];
    char buf[SIZE];

    // check command line arguments
    if (argc != 2)
    {
        fprintf(stderr, "use: %s <port>\n", argv[0]);
        exit(1);
    }
    portno = atoi(argv[1]);

    // create parent socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
        err("error creating socket");

    // setsockopt allows for immediate rerunning of server after killing it
    optval = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (const void *)&optval, sizeof(int));

    // build server's internet address
    bzero((char *) &servAddr, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servAddr.sin_port = htons((unsigned short) portno);

    // bind parent socket with port
    if (bind(sockfd, (struct sockaddr *) &servAddr, sizeof(servAddr)) < 0)
        err("error binding");

    clientLen = sizeof(clientAddr);
    while (1)
    {
        // receive UDP datagram from client
        bzero(buf, SIZE);
        n = recvfrom(sockfd, buf, SIZE, 0, (struct sockaddr *) &clientAddr, &clientLen);
        if (n < 0)
            err("error in recvfrom");
        
        hostPtr = gethostbyaddr((const char *) &clientAddr.sin_addr.s_addr, sizeof(clientAddr.sin_addr.s_addr), AF_INET);
        if (hostPtr == NULL)
            err("error in gethostbyaddr");

        token[0] = strtok(buf, " ");
        int i = 0;
        while (token[i] != NULL)
        {
            i++;
            token[i] = strtok(NULL, " ");
        }

        printf("address %s\n", token[0]);
        printf("port %s\n", token[1]);

        // Create socket
        clientSockfd = socket(AF_INET, SOCK_DGRAM, 0);
        if (clientSockfd < 0)
            err("error opening socket");

        serverPtr = gethostbyname(token[0]);
        if (serverPtr == NULL)
        {
            fprintf(stderr, "error no such host");
            exit(0);
        }
        
        // build server's internet address
        bzero((char *) &hostAddr, sizeof(hostAddr));
        servAddr.sin_family = AF_INET;
        bcopy((char *)serverPtr->h_addr, (char *)&hostAddr.sin_addr.s_addr, serverPtr->h_length);
        hostAddr.sin_port = htons(HOST_PORT);
        
        // send message to server
        serverLen = sizeof(hostAddr);
        n = sendto(clientSockfd, token[1], strlen(token[1]), 0, (struct sockaddr *) &hostAddr, serverLen);
        if (n < 0)
            err("error in sending to server");
        
        int nread = read(clientSockfd, buf, SIZE);
        printf("%s\n", buf);
        
        // echo back to client
        n = sendto(sockfd, buf, strlen(buf), 0, (struct sockaddr *) &clientAddr, clientLen);
        if (n < 0)
            err("error sending to client");
        
        bzero(buf, SIZE);
    }

    return 0;
}

