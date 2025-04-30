//
// Created by amin on 4/30/25.
//
#include <netdb.h>
#include <pthread.h>
#include <stdlib.h>
#include <netdb.h>
#include <stdbool.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#define MAX_BUFFER_SIZE 1024
int main(int argc, char *argv[]) {

    if (argc < 4) {
        fprintf(stderr, "Usage: %s <host> <port> <service>\n", argv[0]);
        exit(1);
    }
    struct sockaddr_in addr = {};
    int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    struct hostent *host = gethostbyname(argv[1]);
    addr.sin_family = AF_INET;
    memcpy(&addr.sin_addr.s_addr, host->h_addr, host->h_length);
    addr.sin_port = htons(atoi(argv[2]));
    if (connect(sock_fd, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
        fprintf(stderr, "error in connection");
        exit(1);
    }
    char buff[MAX_BUFFER_SIZE]="Bonjour";
    write(sock_fd,buff,MAX_BUFFER_SIZE);
    char service[MAX_BUFFER_SIZE];
    strcpy(service,argv[3]);
    char *msg = strcat(service," ");
        if (strncmp(service,"send",4)==0) {
        char *filename=argv[4] ;
        msg = strcat(msg,filename);
    }
    write(sock_fd,msg,strlen(msg));

    char buf[MAX_BUFFER_SIZE]={};
    while (true) {
        read(sock_fd,buf,MAX_BUFFER_SIZE);
        printf("%s\n",buf);
        if (strncmp(buf,"Au revoir",strlen("Au revoir")) == 0) {
            close(sock_fd);
            return 0;
        }
        memset(buf,0,MAX_BUFFER_SIZE);
    }
}
