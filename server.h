#ifndef TCP_ECHO_SERVER_H
#define TCP_ECHO_SERVER_H

// Port range 
#define DEFAULT_PORT 2345
#define MIN_PORT       1
#define MAX_PORT   65535

// Buffer size for each recv() call 
#define BUFFER_SIZE 1024

// Bundles per client data for the thread
typedef struct {
    int  client_fd;
    int  verbose;
} client_info_t;

// Prints usage
void print_usage(const char *progname);

// Creates, binds, and listens on a socket; exits on error
int  setup_server_socket(int port);

// Thread entry: echoes each line back to the client
void *handle_client(void *arg);

#endif