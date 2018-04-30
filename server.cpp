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
    
    memset((char *) &server_addr, 0, sizeof(server_addr));
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
                        char option[40], buff[50];
                        sscanf(message, "%s", option);
                        
                        if (strcmp ("login", option) == 0) {
                            
                            int card_no, account_pin;
                            char to_ignore[20];
                            sscanf(message, "%s %d %d", to_ignore, &card_no, &account_pin);

                            // daca am client cu socket corespunzator - sesiunea e deschisa deja
                            int index = searchForClientBySocket(clients, i);
                            if (index != -1) {
                                strcpy(buff, "IBANK> -2: Sesiune deja deschisa.\n");
                                write (i, buff, strlen(buff));
                                continue;
                            }

                            // daca exista client cu numarul cardului
                            index = searchForClientByCard(clients, card_no);
                            if (index == -1) {
                                strcpy(buff, "IBANK> -4: Numar card inexistent.\n");
                                write (i, buff, strlen(buff));
                                continue;
                            }
                            int client_index = index;
                            // daca contul e blocat
                            if (clients[client_index].blocked) {
                                strcpy(buff, "IBANK> -5: Card blocat.\n");
                                write (i, buff, strlen(buff));
                                continue;
                            }

                            // daca exista client cu pinul dat
                            index = searchForClientByPin(clients, account_pin);
                            if (index == -1) {
                                strcpy(buff, "IBANK> -3: Pin gresit.\n");
                                ++clients[client_index].incercari;
                                if (clients[client_index].incercari == 3)
                                    clients[client_index].blocked = true;
                                write (i, buff, strlen(buff));
                                continue;
                            }

                            index = searchForClientByCardAndSocket(clients, card_no);
                            if (index == -1) {
                                strcpy(buff, "IBANK> -2: Sesiune deja deschisa.\n");
                                write (i, buff, strlen(buff));
                                continue;
                            }

                            sprintf(buff, "IBANK> Welcome %s %s.\n", 
                                clients[client_index].nume, clients[client_index].prenume);
                            clients[client_index].s = i;
                            write(i, buff, strlen(buff));

                        } else if (strcmp ("logout", option) == 0) {
                            
                            int index = searchForClientBySocket(clients, i);
                            if (index == -1) {
                                strcpy(buff, "IBANK> -1: Clientul nu este autentificat.\n");
                                write (i, buff, strlen(buff));
                                continue;
                            }
                            clients[index].s = -1;
                            strcpy(buff, "IBANK> Clientul a fost deconectat\n");
                            write (i, buff, strlen(buff));

                        } else if (strcmp ("listsold", option) == 0) {
                            int index = searchForClientBySocket(clients, i);
                            if (index == -1) {
                                strcpy(buff, "IBANK> -1: Clientul nu este autentificat.\n");
                                write (i, buff, strlen(buff));
                                continue;
                            }
                            sprintf(buff, "IBANK> %.2lf\n", clients[index].sold);
                            write (i, buff, strlen(buff));

                        } else if (strcmp ("transfer", option) == 0) {
                            char to_ignore[20];
                            int card_no;
                            double sum;
                            sscanf(message, "%s %d %lf", to_ignore, &card_no, &sum);

                            int curr_index = searchForClientBySocket(clients, i);
                            if (curr_index == -1) {
                                strcpy(buff, "IBANK> -1: Clientul nu este autentificat.\n");
                                write (i, buff, strlen(buff));
                                continue;
                            }

                            int index = searchForClientByCard(clients, card_no);
                            if (index == -1) {
                                strcpy(buff, "IBANK> -4: Numar card inexistent.\n");
                                write (i, buff, strlen(buff));
                                continue;
                            }
                            if (sum > clients[curr_index].sold) {
                                strcpy(buff, "IBANK> -8: Fonduri insuficiente.\n");
                                write (i, buff, strlen(buff));
                                continue;
                            }
                            if (sum <= 0) {
                                strcpy(buff, "IBANK> -6: Tranzactie esuata.\n");
                                write (i, buff, strlen(buff));
                                continue;
                            }

                            sprintf (buff, "Transfer %lf catre %s %s? [y/n]", sum, 
                                clients[index].nume, clients[index].prenume);
                            write (i, buff, strlen(buff));

                            res = read (i, message, MAX_LEN);
                            if (res == -1)
                                fprintf(stderr, "Reading error!\n");
                            else if (res > 0) {
                                if (strcmp ("y", message) == 0) {
                                    clients[curr_index].sold -= sum;
                                    clients[index].sold += sum;
                                    strcpy (buff, "IBANK> Transfer realizat cu succes.\n");
                                    write (i, buff, strlen(buff));
                                } else {
                                    strcpy(buff, "IBANK> -9: Operatie anulata.\n");
                                    write (i, buff, strlen(buff));
                                    continue;
                                }
                            }
                        }
                     }
                }
            }
        }
    }

    return 0;
}