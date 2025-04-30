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
#define MAX_BUFFER_SIZE
int main(int argc, char *argv[]) {

    if (argc != 3) {
        fprintf(stderr, "Usage: %s <host> <port>\n", argv[0]);
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
    char buf[MAX_BUFFER_SIZE]={};
    while (true) {
        read(sock_fd,buf,MAX_BUFFER_SIZE);
        printf("%s\n",buf);
        if (strcmp(buf,"Au revoir") == 0) {
            close(sock_fd);
            return 0;
        }
        memset(buf,0,MAX_BUFFER_SIZE);
    }
}
