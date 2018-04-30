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
#include <iostream>

#include "Utils.h"

using namespace std;

int main (int argc, char *argv[]) {
    
    if (argc != 3) {
        printf("Usage: %s <port> <data>\n", argv[0]);
        exit(1);
    }
    struct sockaddr_in server_addr, client_addr;
    vector<client> clients;

    FILE *file = fopen (argv[2], "r");
    int people;
    fscanf(file, "%d", &people);
    for (int k = 0; k < people; ++k) {
        client cl;
        fscanf(file, "%s %s %d %d %s %f", cl.nume, cl.prenume, 
                &(cl.cardID), &(cl.pin), cl.secret_pass, &(cl.sold));
        clients.push_back(cl);
    }
    fclose(file);
    
    memset((struct sockaddr_in *) &server_addr, 0, sizeof(server_addr));
    int sock_fd_server, socket_udp;

    fd_set fd_read, fd_server, fd_client;
    
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
    listen(sock_fd_server, 1);
    FD_ZERO(&fd_read);
    FD_SET(sock_fd_server, &fd_read);
    FD_SET(socket_udp, &fd_read);
    FD_SET(0, &fd_read);

    char message[MAX_LEN], buffer[MAX_LEN], arr_fd[MAX_CLIENTS];
    
    int clients_number = 0;
    for (;;) {
        fd_server = fd_read;
        select(FD_SETSIZE, &fd_server, NULL, NULL, NULL);
        char buff[100], option[40];
        for (int i = 0; i < FD_SETSIZE; ++i) {
            if (FD_ISSET(i, &fd_server)) {
                if (socket_udp == i) {
                    socklen_t len_client = sizeof(client_addr);
                    int reading = recvfrom(socket_udp, buff, 100, 0, 
                        (struct sockaddr*) &client_addr, (socklen_t *) &len_client);
                    buff[reading] = '\0';
                    sscanf(buff, "%s", option);

                    if (strcmp ("unlock", option) == 0) {
                        int id_card;
                        char to_ignore[40], pass[40];
                        sscanf(buff, "%s %d %s", to_ignore, &id_card, pass);
                        int clientIndex = searchForClient(clients, pass, id_card);
                        if (clientIndex != -1) {
                            clients[clientIndex].blocked = false;
                            char dump[100];
                            strcpy (dump, "UNLOCK> Client deblocat.\n");
                            int to_send = sendto(socket_udp, dump, strlen(dump), 0, (struct sockaddr*) &client_addr, len_client);
                            if (to_send < 0) {
                                fprintf(stderr, "Sending failed!\n");
                                exit(69);
                            }
                        } else {
                            char dump[100];
                            strcpy (dump, "UNLOCK> Deblocare esuata.\n");
                            int to_send = sendto(socket_udp, dump, strlen(dump), 0, (struct sockaddr*) &client_addr, len_client);
                            if (to_send < 0) {
                                fprintf(stderr, "Sending failed!\n");
                                exit(69);
                            }
                        }
                    }
                } else if (sock_fd_server == i) {
                    
                }
            }
        }
    }

    return 0;
}