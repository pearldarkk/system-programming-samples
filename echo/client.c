#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include "tlpi_hdr.h"

static void usageError(const char *progName, const char *msg) {
    if (msg != NULL)
        fprintf(stderr, "%s\n", msg);
    fprintf(stderr, "Usage: %s IPv4-server-address port\n", progName);
    exit(EXIT_FAILURE);
}

int main(int argc, char **argv) {
    if (argc != 3) {
        usageError(argv[0], NULL);
        exit(EXIT_FAILURE);
    }

    // config remote address
    struct addrinfo hints;
    memset(&hints, 0, sizeof hints);
    hints.ai_socktype = SOCK_STREAM;
    struct addrinfo *peer_address;
    if (getaddrinfo(argv[1], argv[2], &hints, &peer_address)) {
        fprintf(stderr, "getaddrinfo() failed with error code 0x%x", errno);
        exit(EXIT_FAILURE);
    }
    char address_buffer[100];
    char service_buffer[100];
    getnameinfo(peer_address->ai_addr, peer_address->ai_addrlen, address_buffer, sizeof address_buffer, service_buffer, sizeof service_buffer, NI_NUMERICHOST);

    // create socket
    int sock_peer;
    sock_peer = socket(peer_address->ai_family, peer_address->ai_socktype, peer_address->ai_protocol);
    if (sock_peer < 0) {
        fprintf(stderr, "socket() failed with error code 0x%x", errno);
        exit(EXIT_FAILURE);
    }

    // connect to server
    if (connect(sock_peer, peer_address->ai_addr, peer_address->ai_addrlen)) {
        fprintf(stderr, "connect() failed with error code 0x%x", errno);
        exit(EXIT_FAILURE);
    }
    freeaddrinfo(peer_address);
    char read[1024];
    while (1) {
        printf("You: ");
        fgets(read, 1024, stdin);
        send(sock_peer, read, strlen(read), 0);
        if (strncmp(read, "exit()", 6) == 0) {
            close(sock_peer);
            printf("Disconnected from server\n");
            exit(EXIT_FAILURE);
        }
        if (recv(sock_peer, read, 1024, 0) < 0)
            printf("Error receiving data\n");
        else
            printf("Server: %s", read);
    }
    exit(EXIT_SUCCESS);
}