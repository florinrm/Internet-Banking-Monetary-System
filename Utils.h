#ifndef UTILS
#define UTILS

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

struct client {
    char nume[MAX_NAME];
    char prenume[MAX_NAME];
    int cardID;
    int pin;
    char secret_pass[8];
    double sold;
    bool blocked = false;
};

#endif