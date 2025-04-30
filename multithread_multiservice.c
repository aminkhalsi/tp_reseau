//
// Created by amin on 4/30/25.
//
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>
#define MAX_BUFFER_SIZE 1024
void *send_time(int );
void *send_count(int);
void *send_file(const char * ,int);
void * handle_user(void *arg);

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
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

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

        int *connfd = malloc(sizeof(int));
        *connfd = client_fd;
        pthread_create(&tid,NULL,handle_user,connfd);
        pthread_detach(tid);
    }

    close(server_fd);
    return 0;
}
void *send_time(int connfd) {
    char msg[64];
    time_t now;
    struct tm *local;
    for (int n = 0; n < 60; n++) {
        now = time(NULL);
        local = localtime(&now);
        snprintf(msg, sizeof(msg), "Il est: %02d:%02d:%02d\n",
                 local->tm_hour, local->tm_min, local->tm_sec);
        write(connfd, msg, MAX_BUFFER_SIZE);
        sleep(1);
    }
    return NULL;
}
void *send_count(int connfd) {
    FILE *fp;
    char output[MAX_BUFFER_SIZE];
    int count = 0;

    fp = popen("ps aux | wc -l", "r");
    if (fp == NULL) {
        write(connfd, "Error executing command\n", MAX_BUFFER_SIZE);
        return NULL;
    }

    if (fgets(output, sizeof(output), fp) != NULL) {
        count = atoi(output);
    }

    pclose(fp);

    char msg[64];
    snprintf(msg, sizeof(msg), "Il y a %d processus\n", count);
    write(connfd, msg, MAX_BUFFER_SIZE);
    return NULL;
}
void *send_file(const char * filename,int connfd) {
    FILE *fp = fopen(filename,"r");
    if (fp == NULL) {
        fprintf(stderr,"error opening file");
        return NULL;
    }
    char buf[MAX_BUFFER_SIZE];
    while (fgets(buf, sizeof(buf), fp) != NULL) {
        write(connfd, buf, MAX_BUFFER_SIZE);
        memset(buf,0,MAX_BUFFER_SIZE);
    }
    fclose(fp);
    return NULL;
}
void * handle_user(void *arg) {
    int connfd = *(int *)arg;

    char greet_buf[MAX_BUFFER_SIZE] = {0};
    ssize_t bytes_read = read(connfd, greet_buf, sizeof(greet_buf));
    if (bytes_read > 0) {
        printf("Client: %s\n", greet_buf);
    }
    //read request and choose wich function to execute
    char request[MAX_BUFFER_SIZE] = {0};
     read(connfd, request, MAX_BUFFER_SIZE);
    if (strncmp(request, "time",4) == 0) {
        send_time(connfd);
    }
    else if (strncmp(request, "count",5) == 0) {
        send_count(connfd);
    }
    else if (strncmp(request,"send",4) == 0) {
        char *filename=(char*)malloc(strlen(request)-4);
        strcpy(filename,request+5);
        send_file(filename,connfd);
        free(filename);
    }
    else {
        write(connfd,"bad request",MAX_BUFFER_SIZE);
        write(connfd,"Au revoir",MAX_BUFFER_SIZE);
        close(connfd);
        return NULL;
    }
    write(connfd, "Au revoir", MAX_BUFFER_SIZE);
    close(connfd);
    free(arg);
    return NULL;

}