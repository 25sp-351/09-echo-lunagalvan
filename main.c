#include "server.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>

int main(int argc, char *argv[]) {
    int  port    = DEFAULT_PORT;
    int  verbose = 0;
    int  opt;

    // Argument parsing 
    while ((opt = getopt(argc, argv, "p:vh")) != -1) {
        switch (opt) {
            case 'p': {
                char *endptr;
                errno = 0;
                long val = strtol(optarg, &endptr, 10);
                if (errno != 0 || *endptr != '\0'
                    || val < MIN_PORT || val > MAX_PORT) {
                    fprintf(stderr, "Invalid port number: %s\n", optarg);
                    print_usage(argv[0]);
                    exit(EXIT_FAILURE);
                }
                port = (int)val;
                break;
            }
            case 'v':
                verbose = 1;
                break;
            case 'h':
            default:
                print_usage(argv[0]);
                exit(opt == 'h' ? EXIT_SUCCESS : EXIT_FAILURE);
        }
    }

    //  Socket setup 
    int server_fd = setup_server_socket(port);
    printf("Listening on port %d %s\n",
        port, verbose ? "(verbose)" : "");

    // Accept loop
    while (1) {
        struct sockaddr_in client_addr;
        socklen_t addrlen = sizeof(client_addr);

        int client_fd = accept(server_fd,
                               (struct sockaddr *)&client_addr,
                            &addrlen);
        if (client_fd < 0) {
            perror("accept");
            continue;
        }

        if (verbose) {
            char ip[INET_ADDRSTRLEN];
            inet_ntop(AF_INET,
                    &client_addr.sin_addr,
                    ip, sizeof(ip));
            printf("Accepted from %s:%d → fd %d\n",
                ip,
                ntohs(client_addr.sin_port),
                client_fd);
        }

        client_info_t *info = malloc(sizeof(*info));
        if (!info) {
            perror("malloc");
            close(client_fd);
            continue;
        }
        info->client_fd = client_fd;
        info->verbose   = verbose;

        pthread_t tid;
        if (pthread_create(&tid, NULL, handle_client, info) != 0) {
            perror("pthread_create");
            close(client_fd);
            free(info);
            continue;
        }
        pthread_detach(tid);
    }

    close(server_fd);
    return 0;
}