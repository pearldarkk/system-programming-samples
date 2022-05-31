#include <arpa/inet.h>
#include <ctype.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include "tlpi_hdr.h"

static void usageError(const char *progName, const char *msg) {
    if (msg != NULL)
        fprintf(stderr, "%s\n", msg);
    fprintf(stderr, "Usage: %s IPv4-address port\n", progName);
    exit(EXIT_FAILURE);
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
    int sock_listen;
    sock_listen = socket(bind_address->ai_family, bind_address->ai_socktype, bind_address->ai_protocol);

    if (sock_listen < 0) {  // invalid socket
        fprintf(stderr, "socket() failed with error code 0x%x", errno);
        exit(EXIT_FAILURE);
    }
    // bind socket
    if (bind(sock_listen, bind_address->ai_addr, bind_address->ai_addrlen)) {
        fprintf(stderr, "bind() failed with error code 0x%x", errno);
        exit(EXIT_FAILURE);
    }
    freeaddrinfo(bind_address);

    // enter listening state
    if (listen(sock_listen, 10) < 0) {  // accept up to 10 clients a time
        fprintf(stderr, "listen() failed with error code 0x%x", errno);
        exit(EXIT_FAILURE);
    }

    char read[1024];
    struct sockaddr_storage client_address;
    socklen_t client_len = sizeof client_address;
    int pid;
    char address_buffer[100];
    int sock_client;
    // wait for connections
    while (1) {
        if ((sock_client = accept(sock_listen, (struct sockaddr *)&client_address, &client_len)) < 0) {
            fprintf(stderr, "accept() failed with error code 0x%x", errno);
            exit(EXIT_FAILURE);
        }
        if (sock_client < 0)
            exit(EXIT_FAILURE);

        getnameinfo((struct sockaddr *)&client_address, client_len, address_buffer, sizeof address_buffer, 0, 0, NI_NUMERICHOST);
        printf("IP %s connected to server\n", address_buffer);

        if ((pid = fork()) == 0) {
            // child process
            close(sock_listen);
            while (1) {
                int bytes_received = recv(sock_client, read, 1024, 0);
                if (bytes_received < 1) {
                    close(sock_client);
                    exit(EXIT_SUCCESS);
                }
                if (strncmp(read, "exit()", 6) == 0) {
                    printf("IP %s disconnected\n", address_buffer);
                    exit(EXIT_SUCCESS);
                }
                // echo
                send(sock_client, read, bytes_received, 0);
            }
        }
        close(sock_client);
    }
    // close listening socket
    close(sock_listen);
    printf("Server closed");
}