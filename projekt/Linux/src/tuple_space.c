#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include "tuple.h"
#include "tuple_space.h"


char SERVER[16];
int PORT;

void setSocket(char* ip, int port) {
    memcpy(SERVER, ip, strlen(ip));
    PORT = port;
}


void display_message_attributes(int id, field_t* fields, int num_fields, char* buffer, int buffer_len, int message_type) {
    printf("\n----Message-Attributes----\n");
    if(message_type == TS_OUT) {
        printf("Message Type: %d\n", message_type);
        printf("ID: %d\n", id);
        printf("Number of fields: %d\n", num_fields);
        for(int i = 0; i < num_fields; i++) {
            printf("Field %d type: %d\n", i, fields[i].dataType);
            switch (fields[i].dataType) {
                case TYPE_INT:
                    printf("Field %d value: %d\n", i, fields[i].data.int_type);
                    break;
                case TYPE_FLOAT:
                    printf("Field %d value: %f\n", i, fields[i].data.float_type);
                    break;
            }
        }

        printf("ALP Message Content:\n");
        for (int i = 0; i < buffer_len; i++) {
            printf("%02x ", (unsigned char)buffer[i]);
        }
        printf("\n");
    }
    else {
        printf("Message Type: %d\n", message_type);
        printf("ID: %d\n", id);
        printf("Number of fields: %d\n", num_fields);
        for(int i = 0; i < num_fields; i++) {
            printf("Field %d type: %d\n", i, fields[i].dataType);
        }

        printf("ALP Message Content:\n");
        for (int i = 0; i < buffer_len; i++) {
            printf("%02x ", (unsigned char)buffer[i]);
        }
        printf("\n");
    }

}


void send_message_to_server(char* msg, int len) {
    int sockfd;
    struct sockaddr_in servaddr;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Nie można utworzyć gniazda");
        exit(1);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    servaddr.sin_addr.s_addr = inet_addr(SERVER);

    sendto(sockfd, msg, len, 0, (const struct sockaddr *) &servaddr, sizeof(servaddr));
    //close(sockfd);
}


int send_and_receive_message(char* msg, char* rcv_msg, int len) {
    int sockfd;
    struct sockaddr_in server_addr, cliaddr;
    socklen_t sock_len;

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Błąd przy tworzeniu gniazda");
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr(SERVER);

    sendto(sockfd, msg, len, 0, (const struct sockaddr *) &server_addr, sizeof(server_addr));

    sock_len = sizeof(cliaddr);
    int n = recvfrom(sockfd, rcv_msg, 1024, 0, (struct sockaddr *) &server_addr, &sock_len);
    if(n == -1) {
        printf("Message Error");
    }
    printf("\n");

    close(sockfd);

    return n;
}


int ts_out(int id, field_t* fields, int num_fields) {
    int len = 0;
    char buffer[1024];

    buffer[sizeof(int)] = TS_OUT;

    *((int*)(buffer)) = id;
    len += sizeof(int) + 1;

    buffer[len++] = (char)num_fields;
    for(int i = 0; i < num_fields; i++) {
        buffer[len++] = (char)fields[i].dataType;
    }
    for(int j = 0; j < num_fields; j++) {
        switch (fields[j].dataType) {
            case TYPE_INT:
                memcpy(buffer + len, (void*)&fields[j].data.int_type, sizeof(int));
                len += sizeof(int);
                break;
            case TYPE_FLOAT:
                memcpy(buffer + len, (void*)&fields[j].data.float_type, sizeof(float));
                len += sizeof(float);
                break;
        }
    }

    display_message_attributes(id, fields, num_fields, buffer, len, TS_OUT);

    send_message_to_server(buffer, len);

    return TS_SUCCESS;
}


int ts_inp(int id, field_t* fields, int num_fields) {
    int len = 0;
    char buffer[1024];
    char received_buffer[1024];
    int received_message_len = 0;
    protocol_header* header;
    void* data_position;
    char *current_type;

    buffer[sizeof(int)] = TS_INP;

    *((int*)(buffer)) = id;
    len += sizeof(int) + 1;

    buffer[len++] = (char)num_fields;
    for(int i = 0; i < num_fields; i++) {
        buffer[len++] = (char)fields[i].dataType;
    }

    do {
        received_message_len = send_and_receive_message(buffer, received_buffer, len);
        if(received_message_len == 1) {
            usleep(5000000);
            continue;
        }
        break;
    } while(1);

    for (int i = 0; i < received_message_len; i++) {
        printf("%02x ", (unsigned char)received_buffer[i]);
    }
    printf("\n");

    header = (protocol_header*) received_buffer;

    current_type = &header->data_type;
    data_position = current_type + num_fields;
    for(int j=0; j<num_fields; j++) {
        switch (fields[j].dataType) {
            case TYPE_INT:
                memcpy(&fields[j].data.int_type, data_position, sizeof(int));
                data_position += sizeof(int);
                break;
            case TYPE_FLOAT:
                memcpy(&fields[j].data.float_type, data_position, sizeof(float));
                data_position += sizeof(float);
                break;
        }
    }

    return TS_SUCCESS;
}


int ts_rdp(int id, field_t* fields, int num_fields) {
    int len = 0;
    char buffer[1024];
    char received_buffer[1024];
    int received_message_len = 0;
    protocol_header* header;
    void* data_position;
    char *current_type;

    buffer[sizeof(int)] = TS_RDP;

    *((int*)(buffer)) = id;
    len += sizeof(int) + 1;

    buffer[len++] = (char)num_fields;
    for(int i = 0; i < num_fields; i++) {
        buffer[len++] = (char)fields[i].dataType;
    }

    do {
        received_message_len = send_and_receive_message(buffer, received_buffer, len);
        if(received_message_len == 1) {
            usleep(5000000);
            continue;
        }
        break;
    } while(1);

    for (int i = 0; i < received_message_len; i++) {
        printf("%02x ", (unsigned char)received_buffer[i]);
    }
    printf("\n");

    header = (protocol_header*) received_buffer;

    current_type = &header->data_type;
    data_position = current_type + num_fields;
    for(int j=0; j<num_fields; j++) {
        switch (fields[j].dataType) {
            case TYPE_INT:
                memcpy(&fields[j].data.int_type, data_position, sizeof(int));
                data_position += sizeof(int);
                break;
            case TYPE_FLOAT:
                memcpy(&fields[j].data.float_type, data_position, sizeof(float));
                data_position += sizeof(float);
                break;
        }
    }

    return TS_SUCCESS;
}

