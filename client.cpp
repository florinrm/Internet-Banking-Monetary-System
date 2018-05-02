#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <iostream>

#include "Utils.h"

#define MSG_SIZE 80

using namespace std;

bool wait_for_pass = false;
int last_card_user = 0, udp;
struct sockaddr_in server_addr, client_addr;

void checkForUnlock (char *buffer) {
	char message[50];
	sprintf(message, "%s %d", buffer, last_card_user);
	wait_for_pass = true;
	int to_send = sendto(udp, message, strlen(message), 0, 
		(struct sockaddr *) &client_addr, sizeof(sockaddr_in));
	if(to_send < 0) {
		fprintf(stderr, "Sending failed!\n");
		exit (EXIT_FAILURE);
	}
}

void doUnlock (char *buffer) {
	char message[50];
	wait_for_pass = false;
	sprintf(message, "%d %s", last_card_user, buffer);
	int to_send = sendto(udp, message, strlen(message), 0, 
		(struct sockaddr *) &client_addr, sizeof(sockaddr_in));
	if(to_send < 0) {
		fprintf(stderr, "Sending failed!\n");
		exit (EXIT_FAILURE);
	}
}

int main(int argc, char *argv[]) {
	if(argc != 3) {
		printf("Usage : %s <port> <data>\n", argv[0]);
		exit(0);
	}

	char log[50];
	char buffer[MSG_SIZE], message[MSG_SIZE], buf[1024];

	sprintf(log, "client-%d.log", getpid());
	FILE * logfile = fopen(log, "w");


	struct hostent *host;
	
	fd_set fd_server, fd_client;

	fflush(stdout);

	host = gethostbyname(argv[1]);
	client_addr.sin_family = AF_INET;
	client_addr.sin_port = htons(atoi(argv[2]));
	client_addr.sin_addr = *(struct in_addr *) *host->h_addr_list;
	udp = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	int sock_fd = socket(AF_INET, SOCK_STREAM, 0);

	if (connect(sock_fd, (struct sockaddr *) &client_addr, sizeof(client_addr)) < 0) {
		fprintf(stderr, "Cannot connect!\n");
        fprintf(logfile, "Cannot connect!\n");
        exit(69); // wink wink
	}

	fflush(stdout);

	FD_ZERO(&fd_client);
	FD_SET(sock_fd, &fd_client);
	FD_SET(udp, &fd_client);
	FD_SET(0, &fd_client);

	for (;;) {
		fd_server = fd_client;

		select(FD_SETSIZE, &fd_server, NULL, NULL, NULL);
		
		for (int i = 0; i < FD_SETSIZE; ++i) {
			
			socklen_t len_client = sizeof(client_addr);

			if (FD_ISSET(i, &fd_server)) {
				if(i == udp) {
					int dim = recvfrom(udp, buf, 1024, 0, (struct sockaddr*) &server_addr, &len_client);
					buf[dim] = '\0';
					cout << buf << endl;
					char dummy[50];
					sscanf(buf, "%s", dummy);
					if (strncmp(buf, "Trimite", 7) == 0)
						wait_for_pass = true;
				
				} else if (i == sock_fd) {
				
					if (strcmp(message, "Shutdown") == 0) {
						fclose(logfile);
						close(sock_fd);
						printf("%s", message);
						exit(EXIT_SUCCESS);
					}
				
					int result = read(sock_fd, message, MSG_SIZE);
					message[result] = '\0';
					fprintf(logfile, "%s", message);
				
					printf("%s", message);
				
				} else if (i == 0) {
				
					fgets(buffer, MSG_SIZE, stdin);
					char option[30];
					sscanf(buffer, "%s", option);
					
					if(strcmp ("unlock", option) == 0) {
						checkForUnlock(buffer);
					} else if (wait_for_pass) {
						doUnlock(buffer);
					}

					if (strcmp(buffer, "quit\n") == 0) {
						close(sock_fd);
						fclose(logfile);
						exit(EXIT_SUCCESS);

					} else {
						printf("lelel: %s\n", buffer);
						char op[30];
						sscanf(buffer, "%s", op);
						if (strcmp(op, "login") == 0) {
							int id, pin;
							char ignore[30];
							sscanf(buffer, "%s %d %d", ignore, &id, &pin);
							last_card_user = id;
						}
						sprintf(message, "%s", buffer);
						write(sock_fd, message, strlen(message));
						fprintf(logfile, "%s\n", message);
					}
				}
			}
		}
	}
	fclose(logfile);
}
