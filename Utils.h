#ifndef UTILS
#define UTILS

#include <vector>

#define NOT_LOGGED_IN -1
#define SESSION_ON -2
#define WRONG_PIN -3
#define NO_CARD_IN -4
#define BLOCKED_CARD -5
#define FAILED_OP -6
#define FAILED_DEBLOCK -7
#define NO_FUNDS -8
#define CANCELLED_OP -9
#define ERROR_F -10

#define MAX_NAME 13
#define MAX_PASS 8

#define MAX_CLIENTS 40
#define MAX_SIZE 80
#define MAX_LEN 1024

using namespace std;

struct client {
    char nume[MAX_NAME];
    char prenume[MAX_NAME];
    int cardID;
    int pin;
    char secret_pass[8];
    double sold;
    bool blocked = false;
    int s = -1; // socket
    int incercari = 0;

    client() {}
    client(char *_nume, char *_prenume, int id, int _pin, char *secret, double _sold) {
        strcpy (nume, _nume);
        strcpy (prenume, _prenume);
        strcpy (secret_pass, secret);
        cardID = id;
        sold = _sold;
        pin = _pin;
    }

};

int searchForClient (vector <client> clients, char *pass, int id) {
    for (size_t i = 0; i < clients.size(); ++i) {
        if (id == clients[i].cardID && strcmp(clients[i].secret_pass, pass) == 0)
            return i;
    }
    return -1;
}

int searchForClientBySocket (vector <client> clients, int sock) {
    for (size_t i = 0; i < clients.size(); ++i) {
        if (sock == clients[i].s)
            return i;
    }
    return -1;
}

int searchForClientByCard (vector <client> clients, int card) {
    for (size_t i = 0; i < clients.size(); ++i) {
        if (card == clients[i].cardID)
            return i;
    }
    return -1;
}

int searchForClientByPin (vector <client> clients, int acc_pin) {
    for (size_t i = 0; i < clients.size(); ++i) {
        if (acc_pin == clients[i].pin)
            return i;
    }
    return -1;
}

int searchForClientByCardAndSocket (vector <client> clients, int card) {
    for (size_t i = 0; i < clients.size(); ++i) {
        if (clients[i].cardID == card && clients[i].s == -1)
            return i;
    }
    return -1;
}

#endif