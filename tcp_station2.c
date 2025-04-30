#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

static void send_time(int);

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <host> <port>\n", argv[0]);
        exit(1);
    }
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(atoi(argv[2]));
    addr.sin_addr.s_addr = htonl(0);
    if (bind(fd, (const struct sockaddr *) &addr, sizeof(addr))<0) {
        fprintf(stderr,"error binding address");
        exit(1);
    }
    listen(fd, 5);
    while (true) {
        struct sockaddr_in client_addr;
        socklen_t socklen = sizeof(client_addr);
        int connfd = accept(fd, (struct sockaddr *) &client_addr, &socklen);
        char greet_buf[1024];
        read(connfd, greet_buf, 1024);
        printf("%s",greet_buf);
        if (connfd < 0)
            continue;
        send_time(connfd);
        close(connfd);
    }
}

void send_time(int connfd) {
    char format[] = "Il est: %i:%i:%i";
    char msg[strlen(format)+1];
    time_t now;
    struct tm *local;
    int n=0;
    while (n<60) {
        now = time(NULL);
        local = localtime(&now);
        sprintf(msg, format, local->tm_hour, local->tm_min, local->tm_sec);
        write(connfd, msg, 1024);
        n++;
        sleep(1);
    }
    write(connfd,"Au revoir",1024);
}
