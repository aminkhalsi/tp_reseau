#include <ctype.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <arpa/inet.h>
#define MAX_LINE_SIZE 1024
#define MAX_BUF_SIZE 8192

int read_usr_req(char *buf) {
    char line[MAX_LINE_SIZE];
    size_t total_size = 0;
    size_t n = 0;
    while (fgets(line,MAX_LINE_SIZE,stdin)) {
        n = strlen(line);
        total_size += n + 1;
        if (total_size < MAX_LINE_SIZE) {
            if (n == 1) {
                strcat(buf, "\r\n");
                return 0;
            }
            if (n < MAX_LINE_SIZE - 2) {
                line[n - 1] = '\0';
                strcat(line, "\r\n");
            }
            strcat(buf, line);
        } else {
            return -1;
        }
        bzero(line,MAX_LINE_SIZE);
    }
    return 0;
}

int main(int argc, char *argv[]) {
    struct hostent *server;
    int sock_fd;
    struct sockaddr_in serv_addr;
    char recv_buf[MAX_BUF_SIZE]={},
            buf[MAX_BUF_SIZE]={},
            req[MAX_BUF_SIZE]={};
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <host> <port>\n", argv[0]);
    }
    // socket ipv4 tcp
    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (inet_pton(AF_INET, argv[1], &serv_addr.sin_addr)) {
        server = gethostbyaddr(&serv_addr.sin_addr, sizeof(serv_addr.sin_addr),AF_INET);
        if (server == NULL) {
            fprintf(stderr, "Error getting host address for %s",argv[1]);
            exit(1);
        }
    } else {
        server = gethostbyname(argv[1]);
        if (server == NULL) {
            fprintf(stderr, "Error getting host address for %s",argv[1]);
            exit(1);
        }
    }
    bcopy(server->h_addr, &serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(atoi(argv[2]));
    if (connect(sock_fd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        fprintf(stderr, "connect");
        exit(1);
    }
    if (read_usr_req(req)<0) {
        fprintf(stderr,"Problem with user input");
        exit(1);
    }
    memcpy(buf, req, strlen(req));
    ssize_t n = write(sock_fd, buf,MAX_BUF_SIZE);
    if (n<0) {
        fprintf(stderr, "Error reading response");
        exit(1);
    }
    while (read(sock_fd, recv_buf,MAX_BUF_SIZE) == MAX_BUF_SIZE) {
        printf(recv_buf);
        bzero(recv_buf,MAX_BUF_SIZE);
    }
    printf(recv_buf);
    close(sock_fd);
    return 0;
}
