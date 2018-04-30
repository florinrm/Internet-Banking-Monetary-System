#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <iostream>

#include "Utils.h"

using namespace std;

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <port> <data>\n", argv[0]);
        exit(1);
    }

    int clientID = getpid();
    char log[50];
    sprintf(log, "client-<%d>.log", clientID);
    FILE *logfile = fopen (log, "w"); // construiesc log-ul de comenzi
    
    int reading, len;
    fd_set fd_server, fd_client;

    struct sockaddr_in client_addr, server_addr;
    struct hostent *host;

    char message[MAX_SIZE], buf[MAX_SIZE], buffer[MAX_LEN];

    int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    host = gethostbyname(argv[1]);

    client_addr.sin_family = AF_INET;
    client_addr.sin_port = htons(atoi(argv[2]));
    client_addr.sin_addr = *(struct in_addr *) *host->h_addr_list;
    int udp_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);


    if (connect(sock_fd, (struct sockaddr *) &client_addr, sizeof(client_addr)) < 0) {
        fprintf(stderr, "Cannot connect!\n");
        fprintf(logfile, "Cannot connect!\n");
        exit(69); // wink wink
    }
    fflush (stdout);

    FD_ZERO(&fd_client);
    FD_SET(sock_fd, &fd_client);
    FD_SET(udp_fd, &fd_client);
    FD_SET(0, &fd_client);

    for (;;) {
        fd_server = fd_client;
        select(FD_SETSIZE, &fd_server, NULL, NULL, NULL);
        for (int i = 0; i < FD_SETSIZE; ++i) {
            socklen_t size_client = sizeof(client_addr);
            if (FD_ISSET(i, &fd_server)) {
                len = recvfrom (udp_fd, buffer, 1024, 0, 
                    (struct sockaddr*) &server_addr, &size_client);
                if (udp_fd == i) {                  
                    buffer[len] = '\0';
                    cout << buffer << endl;
                } else if (sock_fd == i) {
                    if (strcmp (buf, "Shutdown") == 0) {
                        cout << buf << endl;
                        close(sock_fd); // inchid socketul
                        fclose(logfile);
                        exit(69);
                    }
                    reading = read(sock_fd, buf, MAX_SIZE);
                    buf[reading] = '\0';
                    fprintf(logfile, "%s\n", buf);
                    cout << buf << endl;
                } else if (i == 0) {
                    fgets (message, MAX_SIZE, stdin);
                    char option[40];
                    sscanf(message, "%s", option);
                    int to_send = sendto(udp_fd, message, strlen(message), 0, 
                        (struct sockaddr *) &client_addr, sizeof(client_addr));
                    if (strcmp ("unlock", option) == 0) {
                        if (to_send < 0) {
                            fprintf(stderr, "Sending failed!\n");
                            return 0;
                        }
                    } else if (strcmp ("quit\n", message) == 0) {
                        fclose(logfile);
                        close(sock_fd); // inchid socketul
                        exit(69);
                    } else {
                        sprintf(buf, "%s", message);
                        write(sock_fd, buf, strlen(buf));
                        fprintf(logfile, "%s\n", buf);
                        
                    }
                }
            }
        }
        break;
    }
    fclose(logfile);
    return 0;
}