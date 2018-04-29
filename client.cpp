#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include "Utils.h"

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <port> <data>\n", argv[0]);
        exit(1);
    }

    int clientID = getpid();
    char log[50];
    sprintf(log, "client-<%d>.log", clientID);
    FILE *logfile = fopen (log, "w"); // construiesc log-ul de comenzi

    int port = atoi(argv[2]); // portul
    char ip_server[MAX_SIZE];
    strcpy(ip_server, argv[1]); // ip-ul server-ului
    
    int res, fd_set, fd_server, fd_client;

    struct sockaddr_in client_addr, server_addr;
    struct hostent *host;

    char message[MAX_SIZE], buff[MAX_SIZE];

    int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    host = gethostbyname(ip_server);

    client_addr.sin_family = AF_INET;
    client_addr.sin_port = htons(port);
    client_addr.sin_addr = *(struct in_addr *) *host->h_addr_list;
    int udp_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);


    if (connect(sock_fd, (struct sockaddr *) &client_addr, sizeof(client_addr)) < 0) {
        fprintf(stderr, "Cannot connect!");
        exit(69); // wink wink
    }
    fflush (stdout);


    for (;;) {
        fd_server = fd_client;
        break;
    }
    return 0;
}