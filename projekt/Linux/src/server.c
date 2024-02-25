#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include "tuple.h"
#include "tuple_space.h"
//#include "utilities.h"

#define SERVER "0.0.0.0"
#define PORT 5012

tuple_t tuples[MAX_TUPLES];
int tuple_index = 0;


void printMessage(char *msg, ssize_t msg_len) {
    printf("\n-------Message-Binary-------\n");
    fflush(stdout); // Czyszczenie bufora

    for (int i = 0; i < msg_len; i++) {
        printf("%02x ", (unsigned char)msg[i]);
    }
    printf("\n");
}


void printTuple(int index) {
    printf("\n---Tuple---\n");
    printf("Tuple Index: %d\n", index);
    printf("ID: %d\n", tuples[index].id);
    printf("Num of Fields: %d\n", tuples[index].num_fields);
    for(int i=0; i<tuples[index].num_fields; i++) {
        printf("Type: %d, ", tuples[index].data_type[i]);
        printf("Data: %d\n", tuples[index].data[i].int_type);
    }
}


int isTupleSpaceEmpty(){
    return tuple_index == 0;
}


int convertTupleToMsg(int index, char* msg) {
    int len = 0;
    protocol_header* header;

    header = (protocol_header*)msg;

    header->operation_type = TS_INP;

    header->id = tuples[index].id;
    len = sizeof(int) + 2;

    header->num_fields = (char)tuples[index].num_fields;
    for(int i = 0; i < header->num_fields; i++) {
        msg[len++] = tuples[index].data_type[i];
    }
    for(int j = 0; j < header->num_fields; j++) {
        switch (tuples[index].data_type[j]) {
            case TYPE_INT:
                memcpy(msg + len, (void*)&tuples[index].data[j].int_type, sizeof(int));
                len += sizeof(int);
                break;
            case TYPE_FLOAT:
                memcpy(msg + len, (void*)&tuples[index].data[j].int_type, sizeof(float));
                len += sizeof(float);
                break;
        }
    }
    return len;
}


void sendNotFoundTupleMessage(int sockfd, struct sockaddr_in* cliaddr) {
    int len = 1;
    char msg;

    if (sockfd < 0) {
        perror("Nie można utworzyć gniazda");
        exit(1);
    }

    printf("\nNot Found Tuple Message\n");
    msg = 0;

    sendto(sockfd, &msg, len, 0, (const struct sockaddr *)cliaddr, sizeof(struct sockaddr_in));
}


void sendTuple(int index, int sockfd, struct sockaddr_in* cliaddr) {
    int len = 0;
    char msg[1024];

    printf("\nTuple to send:");
    printTuple(index);

    len = convertTupleToMsg(index, msg);
    if (sockfd < 0) {
        perror("Nie można utworzyć gniazda");
        exit(1);
    }

    printMessage(msg, len);
    sendto(sockfd, msg, len, 0, (const struct sockaddr *)cliaddr, sizeof(struct sockaddr_in));

}


void deleteTuple(int index) {
    if(tuple_index == 0) return;
    memcpy(&tuples[index], &tuples[tuple_index-1], sizeof(tuple_t));
    tuple_index--;
}


void decode_message_from_client(char* buffer) {
    char *current_type;
    void* data_position;

    printf("---Message---\n");
    protocol_header* header;
    header = (protocol_header*) buffer;
    printf("ID: %d\n", header->id);
    current_type = &header->data_type;

    printf("Operation Type: %d\n", header->operation_type);

    printf("Num Fields: %d\n", header->num_fields);

    data_position = current_type + header->num_fields;
    for (int i = 0; i < header->num_fields; i++) {

        printf("Data Type: %d\n", *current_type);
        switch (*current_type++) {
            case TYPE_INT:
                printf("Value: %d\n", *((int*)data_position));
                data_position += sizeof(int);
                break;
            case TYPE_FLOAT:
                printf("Value: %f\n", *((float*)data_position));
                data_position += sizeof(float);
                break;
        }
    }
    printf("\n");
}


void addTuple(char* buffer) {
    tuple_t tuple;
    char *current_type;
    void* data_position;

    protocol_header* header;
    header = (protocol_header*) buffer;
    tuple.id = header->id;
    current_type = &header->data_type;

    tuple.num_fields = header->num_fields;

    data_position = current_type + header->num_fields;
    for (int i = 0; i < header->num_fields; i++) {

        tuple.data_type[i] = *current_type++;
        switch (tuple.data_type[i]) {
            case TYPE_INT:
                memcpy((void*)&tuple.data[i].int_type, data_position, sizeof(int));
                data_position += sizeof(int);
                //printf("%d\n", tuple.data[i].int_type);
                break;
            case TYPE_FLOAT:
                memcpy((void*)&tuple.data[i].float_type, data_position, sizeof(float));
                data_position += sizeof(float);
                //printf("%f\n", tuple.data[i].float_type);
                break;
        }
    }
    memcpy((void*)(&tuples[tuple_index]), (void*)&tuple, sizeof(tuple_t));
    tuple_index++;
}


int findTuple(char* buffer) {
    int i,j;
    char *type_pointer;
    protocol_header* header;

    header = (protocol_header*)buffer;

    for(i=0; i<tuple_index; i++) {

        if(tuples[i].id != header->id) continue;
        if(tuples[i].num_fields != header->num_fields) continue;
        type_pointer = &header->data_type;
        for(j = 0; j<header->num_fields; j++) {
            if(tuples[i].data_type[j] != *type_pointer) break;
            type_pointer = type_pointer + 1;
        }
        if(j == header->num_fields) return i;
    }
    return -1;
}


void handleMessage(char* msg, int sockfd, struct sockaddr_in* cliaddr) {
    int returned_tuple_index;
    printf("Zerowa Tupla: ");
    printTuple(0);
    protocol_header* header;
    header = (protocol_header*)msg;
    //printf("Received message from %s:%d\n", inet_ntoa(cliaddr.sin_addr), ntohs(cliaddr.sin_port));
    if(header->operation_type == TS_OUT) {
        addTuple(msg);
    }
    else {
        returned_tuple_index = findTuple(msg);
        //printf("\nIndex: %d\n", returned_tuple_index);
        if(returned_tuple_index == -1) {
            sendNotFoundTupleMessage(sockfd, cliaddr);
        }
        else {
            sendTuple(returned_tuple_index, sockfd, cliaddr);
            if(header->operation_type == TS_INP) deleteTuple(returned_tuple_index);
        }
    }
}


int main() {
    int sockfd;
    struct sockaddr_in servaddr, cliaddr;
    char msg[1024];
    socklen_t len;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Nie można utworzyć gniazda");
        exit(1);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    servaddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(sockfd, (const struct sockaddr *) &servaddr, sizeof(servaddr)) < 0) {
        perror("Błąd przy bind");
        exit(1);
    }
    printf("Server IP: %s\n", inet_ntoa(servaddr.sin_addr));

    int iter = 0;
    while (1) {
        len = sizeof(cliaddr);
        ssize_t n = recvfrom(sockfd, msg, 1024, 0, (struct sockaddr *) &cliaddr, &len);
        // Jak nie ma ramki co się dzieje
        if (n < 0) {
            perror("Błąd w recvfrom");
            continue;
        }
        msg[n] = '\0';

        handleMessage(msg, sockfd, &cliaddr);

        decode_message_from_client(msg);

        iter++;
    }
    return 0;
}