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
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <host> <port>\n", argv[0]);
        exit(1);
    }
    struct sockaddr_in addr = {};
    int sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
    struct hostent *host = gethostbyname(argv[1]);
    addr.sin_family = AF_INET;
    memcpy(&addr.sin_addr.s_addr, host->h_addr, host->h_length);
    addr.sin_port = htons(atoi(argv[2]));
    char buff[1024] = "Bonjour";
    int n=sendto(sock_fd, buff, MAX_BUFFER_SIZE,MSG_CONFIRM, (struct sockaddr *) &addr, sizeof(addr));
    printf("%i\n",n);
    socklen_t socklen=sizeof(addr);
    char buf[MAX_BUFFER_SIZE]={};
    while (true) {
        recvfrom(sock_fd,buf,MAX_BUFFER_SIZE,MSG_CONFIRM,(struct sockaddr *)&addr,&socklen);
        printf("%s\n",buf);
        if (strcmp(buf,"Au revoir") == 0) {
            return 0;
        }
        memset(buf,0,MAX_BUFFER_SIZE);
    }
}
