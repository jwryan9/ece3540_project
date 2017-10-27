                                                                                        
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BUFFER_SIZE 1024
#define PORT 15001

void err(char * msg)
{
    perror(msg);
    exit(0);
}

int main(int argc, char **argv)
{
    int sockfd, numRead, n, serverLen;
    struct sockaddr_in servAddr;
    struct hostent *server;
    char * hostname;
    char * request;
    char buf[BUFFER_SIZE];

    if (argc != 3)
    {
        fprintf(stderr, "use: %s  <hostname> <request>\n", argv[0]);
        exit(0);
    }

    hostname = argv[1];
    request = argv[2];

    strcpy(buf, hostname);
    strcat(buf, " ");
    strcat(buf, request);

    // create socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
        err("error createing socket");

    server = gethostbyname("localhost");
    if (server == NULL)
    {
        fprintf(stderr, "error no host %s\n", hostname);
        exit(0);
    }

    bzero((char *) &servAddr, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    bcopy((char *) server->h_addr, (char *) &servAddr.sin_addr.s_addr, server->h_length);
    servAddr.sin_port = htons(PORT);

    serverLen = sizeof(servAddr);
    n = sendto(sockfd, buf, strlen(buf), 0, (struct sockaddr *) &servAddr, serverLen);
    if (n < 0)
        err("errer sending to server");

    numRead = read(sockfd, buf, BUFFER_SIZE);
    printf("%s\n", buf);

    close(sockfd);
    return 0;
}


