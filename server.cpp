#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <math.h>
#include <algorithm>
#include <vector>

#include "Utils.h"

using namespace std;

int main (int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <port> <data>\n", argv[0]);
        exit(1);
    }
    struct sockaddr_in server_addr, client_addr;
    vector<client> clients;

    memset((struct sockaddr_in *) &server_addr, 0, sizeof(server_addr));
    int sock_fd_server, socket_udp;
    
    sock_fd_server = socket(AF_INET, SOCK_STREAM, 0);
    server_addr.sin_port = htons(atoi(argv[1]));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;

    const int val = 1;
    setsockopt(sock_fd_server, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(int));
    setsockopt(socket_udp, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(int));

    int binding = bind (sock_fd_server, (struct sockaddr *) &server_addr, sizeof(server_addr));
    socket_udp = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    
    if (binding < 0) {
        fprintf(stderr, "UDP binding failed!\n");
        exit(EXIT_FAILURE);
    }

    return 0;
}