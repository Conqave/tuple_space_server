#ifndef TUPLE_SPACE_H
#define TUPLE_SPACE_H

#include "tuple.h"

#define TS_OUT 0
#define TS_INP 1
#define TS_RDP 2

#define MAX_ELEMENTS 10
#define MAX_TUPLES 512

typedef struct {
    unsigned int id;
    int num_fields;
    char data_type[MAX_ELEMENTS];
    union {
        int int_type;
        float float_type;
    } data[MAX_ELEMENTS];
} tuple_t;

typedef struct {
    unsigned int id;
    char operation_type;
    char num_fields;
    char data_type;

} protocol_header;

/*
void setSocket(char* ip, int port);
void printMessage(char *msg, ssize_t msg_len);
void printTuple(int index);
int isTupleSpaceEmpty();
int convertTupleToMsg(int index, char* msg);
void sendNotFoundTupleMessage(int sockfd, struct sockaddr_in* cliaddr);
void sendTuple(int index, int sockfd, struct sockaddr_in* cliaddr);
void deleteTuple(int index);
void decode_message_from_client(char* buffer);
void addTuple(char* buffer);
int findTuple(char* buffer);
*/

#endif