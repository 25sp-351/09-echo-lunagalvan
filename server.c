#include "server.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>

void print_usage(const char *progname) {
    fprintf(stderr,
        "Usage: %s [-p port] [-v]\n"
        "  -p <port>   Port to listen on (default %d, range %d–%d)\n"
        "  -v          Verbose: print each received line\n",
        progname, DEFAULT_PORT, MIN_PORT, MAX_PORT);
}

int setup_server_socket(int port) {
    int fd, opt = 1;
    struct sockaddr_in addr;

    if ((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }
    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("setsockopt");
        close(fd);
        exit(EXIT_FAILURE);
    }

    memset(&addr, 0, sizeof(addr));
    addr.sin_family      = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port        = htons(port);

    if (bind(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("bind");
        close(fd);
        exit(EXIT_FAILURE);
    }
    if (listen(fd, SOMAXCONN) < 0) {
        perror("listen");
        close(fd);
        exit(EXIT_FAILURE);
    }
    return fd;
}

void *handle_client(void *arg) {
    client_info_t *info = (client_info_t *)arg;
    char buffer[BUFFER_SIZE];
    ssize_t n;

    while ((n = read(info->client_fd, buffer, BUFFER_SIZE - 1)) > 0) {
        buffer[n] = '\0';
        if (info->verbose) {
            printf("Received: %s", buffer);
        }
        if (write(info->client_fd, buffer, n) != n) {
            perror("write");
            break;
        }
    }

    if (n == 0 && info->verbose) {
        printf("Client %d disconnected\n", info->client_fd);
    } else if (n < 0) {
        perror("read");
    }

    close(info->client_fd);
    free(info);
    return NULL;
}