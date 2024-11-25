#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define MAX 80
#define PORT 8080
#define SA struct sockaddr

void runtime(int connfd)
{
    for (;;)
    {
        int *temp = NULL, *watt = NULL;
        float *packageUtil = NULL, *totalUtil = NULL;

        export_info(&temp, &watt, &packageUtil, &totalUtil);

        //

        char buffer[256] = {0};

        //

        ssize_t status;

        if (*watt >= 5000) {
            *watt = -1;
        }

        sprintf(buffer, "WATT|%d:TEMP|%d:PUTL|%0.1f:TUTL|%0.1f", *watt, *temp, *packageUtil, *totalUtil);

        status = send(connfd, buffer, strlen(buffer), MSG_NOSIGNAL);
        if (status == -1) {
            close(connfd);
            break;
        }

        usleep(1000000); 
    }
}

void *initialize()
{
    int sockfd, connfd, len;
    struct sockaddr_in servaddr, cli;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        printf("socket creation failed...\n");
        exit(0);
    }
    bzero(&servaddr, sizeof(servaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(PORT);

    if ((bind(sockfd, (SA *)&servaddr, sizeof(servaddr))) != 0)
    {
        printf("socket bind failed...\n");
        exit(0);
    }

    if ((listen(sockfd, 5)) != 0)
    {
        printf("Listen failed...\n");
        exit(0);
    }
    len = sizeof(cli);

    for (;;)
    {
        connfd = accept(sockfd, (SA *)&cli, &len);
        if (connfd < 0)
        {
            printf("server accept failed...\n");
            exit(0);
        }

        runtime(connfd);

        close(connfd);
    }
}