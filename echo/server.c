// gcc ${file} -lpthread -o ...
#include <arpa/inet.h>
#include <ctype.h>
#include <netdb.h>
#include <netinet/in.h>
#include <pthread.h>
#include <signal.h>
#include <sys/socket.h>

#include "tlpi_hdr.h"

int sock_listen;
typedef struct {
    int sock_client;
    char *address_buffer;
} client;

static void usageError(const char *progName, const char *msg) {
    if (msg != NULL)
        fprintf(stderr, "%s\n", msg);
    fprintf(stderr, "Usage: %s IPv4-address port\n", progName);
    exit(EXIT_FAILURE);
}

void *client_handler(void *arg) {
    client *cli = arg;
    char read[1024];

    while (1) {
        int bytes_received = recv(cli->sock_client, read, 1024, 0);
        if (bytes_received < 1) {
            fprintf(stderr, "recv() from %s failed\n", cli->address_buffer);
            break;
        }
        if (strncmp(read, "exit()", 6) == 0) {
            printf("IP %s disconnected from server\n", cli->address_buffer);
            break;
        }
        // echo
        if (send(cli->sock_client, read, bytes_received, 0) < 0) {
            fprintf(stderr, "send() failed\n");
            break;
        }
    }
    close(cli->sock_client);
    return arg;
}

void sigint_handler(int signum) {
    if (signum == SIGINT) {
        // close listening socket
        close(sock_listen);
        printf("\nServer closed\n");
        exit(EXIT_SUCCESS);
    }
}

int main(int argc, char **argv) {
    if (argc != 3) {
        usageError(argv[0], NULL);
        exit(EXIT_FAILURE);
    }

    // config address
    struct addrinfo hints;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    struct addrinfo *bind_address;
    getaddrinfo(argv[1], argv[2], &hints, &bind_address);

    // create socket
    sock_listen = socket(bind_address->ai_family, bind_address->ai_socktype, bind_address->ai_protocol);

    if (sock_listen < 0) {  // invalid socket
        fprintf(stderr, "socket() failed with error code 0x%x\n", errno);
        exit(EXIT_FAILURE);
    }
    // bind socket
    if (bind(sock_listen, bind_address->ai_addr, bind_address->ai_addrlen)) {
        fprintf(stderr, "bind() failed with error code 0x%x\n", errno);
        exit(EXIT_FAILURE);
    }
    freeaddrinfo(bind_address);

    // enter listening state
    if (listen(sock_listen, 10) < 0) {  // accept up to 10 clients a time
        fprintf(stderr, "listen() failed with error code 0x%x\n", errno);
        exit(EXIT_FAILURE);
    }

    printf("Server is listening on (%s, %s)\n", argv[1], argv[2]);

    // register sigint handler
    signal(SIGINT, sigint_handler);

    client cli;
    cli.address_buffer = (char *)malloc(100);
    struct sockaddr_storage client_address;
    socklen_t client_len = sizeof client_address;
    // wait for connections
    while (1) {
        if ((cli.sock_client = accept(sock_listen, (struct sockaddr *)&client_address, &client_len)) < 0) {
            fprintf(stderr, "accept() failed with error code 0x%x\n", errno);
            exit(EXIT_FAILURE);
        }
        if (cli.sock_client < 0)
            exit(EXIT_FAILURE);

        getnameinfo((struct sockaddr *)&client_address, client_len, cli.address_buffer, 100, 0, 0, NI_NUMERICHOST);
        printf("IP %s connected to server\n", cli.address_buffer);

        pthread_t tid;
        if (pthread_create(&tid, NULL, client_handler, &cli.sock_client) == 0)
            pthread_detach(tid);
        else
            perror("Thread creation failed");
    }

    exit(EXIT_SUCCESS);
}