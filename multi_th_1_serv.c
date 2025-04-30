#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>

void *send_time(void *arg);

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int port = atoi(argv[1]);
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(server_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 5) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    while (true) {
        struct sockaddr_in client_addr;
        socklen_t socklen = sizeof(client_addr);
        int client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &socklen);
        if (client_fd < 0) {
            perror("accept");
            continue;
        }

        pthread_t tid;
        int *pclient = malloc(sizeof(int));
        *pclient = client_fd;
        pthread_create(&tid, NULL, send_time, pclient);
        pthread_detach(tid);
    }

    close(server_fd);
    return 0;
}
void *send_time(void *arg) {
    int connfd = *((int *)arg);
    free(arg);

    char greet_buf[1024] = {0};
    ssize_t bytes_read = read(connfd, greet_buf, sizeof(greet_buf) - 1);
    if (bytes_read > 0) {
        printf("Client: %s\n", greet_buf);
    }

    char msg[64];
    time_t now;
    struct tm *local;
    for (int n = 0; n < 60; n++) {
        now = time(NULL);
        local = localtime(&now);
        snprintf(msg, sizeof(msg), "Il est: %02d:%02d:%02d\n",
                 local->tm_hour, local->tm_min, local->tm_sec);
        write(connfd, msg, strlen(msg));
        sleep(1);
    }

    const char *bye = "Au revoir";
    write(connfd, bye, strlen(bye));
    close(connfd);
    return NULL;
}