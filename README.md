# Internet Banking Monetary System

C++ implementation of a simulated Internet banking monetary system, a Linux client - server application, built using TCP and UDP sockets.

## How does it work?
This application represents an interrogation between multiple clients and a server, run in Linux terminals

## How to run the application?
1) Run the makefile for building executables for client and server.
2) Run the server executable: ./server port_number user_data_file (user_data_file represents a list with info about registered clients: surname, name, card_number, pin, password, account_money)
3) Run the client executable(s), in other terminal(s): ./client host_ip port_number
4) Clients can interrogate the server using next commands:
- login card_number pin
- logout - client must be logged in firstly
- listsold - lists the money available in logged in client's account
- transfer card_number money_sum - transfer a sum of money to another client
- unlock - if a client is locked, he / she can unlock his / her account through this command and typing the password
5) Server can send the command quit, in order to quit the clients - server communication
