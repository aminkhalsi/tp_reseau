#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#define MAX_BUFFER_SIZE 1024
void send_time(int,struct sockaddr *);

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <host> <port>\n", argv[0]);
        exit(1);
    }
    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    struct sockaddr_in serv_addr, client_addr;
    socklen_t sock_len = sizeof(client_addr);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(atoi(argv[2]));
    serv_addr.sin_addr.s_addr = htonl(0);
    if (bind(fd, (const struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        fprintf(stderr, "error binding to address");
        exit(1);
    }

    while (true) {
        char greet_buf[MAX_BUFFER_SIZE]={};
        recvfrom(fd,greet_buf,MAX_BUFFER_SIZE,MSG_WAITALL, (struct sockaddr *) &client_addr, &sock_len);
        printf("%s", greet_buf);
        send_time(fd,(struct sockaddr *)&client_addr);
    }
}

void send_time(int sock_fd,struct sockaddr *client_addr) {
    char format[] = "Il est: %i:%i:%i";
    char msg[strlen(format) + 1];
    time_t now;
    struct tm *local;
    int n = 0;
    while (n < 60) {
        now = time(NULL);
        local = localtime(&now);
        sprintf(msg, format, local->tm_hour, local->tm_min, local->tm_sec);
        sendto(sock_fd, msg, MAX_BUFFER_SIZE,MSG_CONFIRM,client_addr,sizeof(*client_addr));
        n++;
        sleep(1);
    }
        sendto(sock_fd, "Au revoir", MAX_BUFFER_SIZE,MSG_CONFIRM,client_addr,sizeof(*client_addr));
}
