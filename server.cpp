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
        printf("Usage: %s <port> <data_file>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    struct sockaddr_in server_addr, client_addr;

    vector<client> clients;

    FILE *file = fopen (argv[2], "r");
    int people;
    fscanf(file, "%d", &people);
    for (int k = 0; k < people; ++k) {
        client cl;
        fscanf(file, "%s %s %d %d %s %lf", cl.nume, cl.prenume, 
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

    char message[MAX_LEN], buff[MAX_LEN];
    int arr_fd[MAX_CLIENTS];
    
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
                        char dump[100];
                        
                        if (clientIndex != -1) {
                            clients[clientIndex].blocked = false;                            
                            strcpy (dump, "UNLOCK> Client deblocat.\n");
                            int to_send = sendto(socket_udp, dump, strlen(dump), 0, (struct sockaddr*) &client_addr, len_client);
                            if (to_send < 0) {
                                fprintf(stderr, "Sending failed!\n");
                                exit(69);
                            }
                        } else {
                            strcpy (dump, "UNLOCK> Deblocare esuata.\n");
                            int to_send = sendto(socket_udp, dump, strlen(dump), 0, (struct sockaddr*) &client_addr, len_client);
                            if (to_send < 0) {
                                fprintf(stderr, "Sending failed!\n");
                                exit(69);
                            }
                        }
                    }
                } else if (sock_fd_server == i) {
                    if (clients_number < MAX_CLIENTS) {
                        int sock_fd_client = accept(sock_fd_server, NULL, NULL);
                        FD_SET(sock_fd_client, &fd_read);
                        arr_fd[clients_number++] = sock_fd_client;
                        fflush(stdout);
                    }
                } else if (i == 0) {
                    fgets(message, MAX_LEN, stdin);
                    if (strcmp ("quit\n", message) == 0) {
                        strcpy (buff, "Shutdown\n");
                        for (int k = 0; k < clients_number; ++k) {
                            write(arr_fd[k], buff, strlen(buff));
                            close(arr_fd[k]);
                        }
                        close(sock_fd_server);
                        exit(69);
                    }
                } else if (i > 0) {
                    int res = read (i, message, MAX_LEN);
                    if (res == -1)
                        fprintf(stderr, "Reading error!\n");
                    else if (res > 0) {
                        message[res] = '\0';
                        /* TODO */
                        char option[40];
                        sscanf(message, "%s", option);
                        if (strcmp ("login", option) == 0) {

                        } else if (strcmp ("logout", option) == 0) {

                        } else if (strcmp ("listsold", option) == 0) {

                        } else if (strcmp ("transfer", option) == 0) {
                            
                        }
                     }
                }
            }
        }
    }

    return 0;
}