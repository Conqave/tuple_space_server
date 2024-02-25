#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <time.h>
#include <sys/socket.h>
#include <errno.h>

#define CLIENT_PORT1 15225 // Port serwera 1
#define CLIENT_PORT2 18566 // Port serwera 2

int main() {
    int client_socket1, client_socket2;
    struct sockaddr_in server1_addr, server2_addr;
    socklen_t addr_len = sizeof(struct sockaddr_in);
    char recv_buffer[2];

    clock_t start, end;
    double diffTime;
    int temp;

    // Inicjalizacja klienta i socketów
    client_socket1 = socket(AF_INET, SOCK_DGRAM, 0);
    client_socket2 = socket(AF_INET, SOCK_DGRAM, 0);

    if (client_socket1 < 0 || client_socket2 < 0) {
        perror("Socket error");
        exit(EXIT_FAILURE);
    }

    // Ustawienia limitu czasu dla odbierania danych
    struct timeval timeout;
    timeout.tv_sec = 5;  // Po 5 sekundach
    timeout.tv_usec = 0;

    if (setsockopt(client_socket1, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0) {
        perror("setsockopt failed\n");
    }

    if (setsockopt(client_socket2, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0) {
        perror("setsockopt failed\n");
    }

    // Ustawienia adresów serwerów
    memset(&server1_addr, 0, addr_len);
    server1_addr.sin_family = AF_INET;
    server1_addr.sin_addr.s_addr = inet_addr("192.168.56.101");
    server1_addr.sin_port = htons(CLIENT_PORT1);

    memset(&server2_addr, 0, addr_len);
    server2_addr.sin_family = AF_INET;
    server2_addr.sin_addr.s_addr = inet_addr("192.168.56.101");
    server2_addr.sin_port = htons(CLIENT_PORT2);

    while (1) {

        // Odbiór danych od serwera 1
        memset(recv_buffer, 0, sizeof(recv_buffer));
        start = clock();
        ssize_t recv_size1 = recvfrom(client_socket1, recv_buffer, sizeof(recv_buffer), 0, (struct sockaddr *)&server1_addr, &addr_len);
        end = clock();
        diffTime = ((double)(end-start)) / (CLOCKS_PER_SEC / 1000);
        if(diffTime > 7800) {
            printf("Serwer 1 nieaktywny\n");
            exit(1);
        }
        if(recv_size1 < 0) {
            if (errno == EWOULDBLOCK) {
                printf("recvfrom timeout\n");
            } else {
                perror("recvfrom error");
            }
            exit(EXIT_FAILURE);
        }
        printf("Odebrano dane od serwera 1: %s\n", recv_buffer);


        // Odbiór danych od serwera 2
        memset(recv_buffer, 0, sizeof(recv_buffer));
        start = clock();
        ssize_t recv_size2 = recvfrom(client_socket2, recv_buffer, sizeof(recv_buffer), 0, (struct sockaddr *)&server2_addr, &addr_len);
        end = clock();
        diffTime = ((double)(end-start)) / (CLOCKS_PER_SEC / 1000);
        if(diffTime > 7800) {
            printf("Serwer 2 nieaktywny\n");
            exit(1);
        }
        if(recv_size2 < 0) {
            if (errno == EWOULDBLOCK) {
                printf("recvfrom timeout\n");
            } else {
                perror("recvfrom error");
            }
            exit(EXIT_FAILURE);
        }
        printf("Odebrano dane od serwera 2: %s\n", recv_buffer);


        // Wysłanie prośby o temperaturę do serwera 2
        memset(recv_buffer, 0, sizeof(recv_buffer));
        recv_buffer[0] = 1;
        recv_buffer[1] = 0;
        ssize_t sent_size = sendto(client_socket2, recv_buffer, sizeof(recv_buffer), 0, (struct sockaddr *)&server2_addr, addr_len);
        if(sent_size < 0) {
            perror("sendto error");
            exit(EXIT_FAILURE);
        }
        printf("Wysłano prośbę o temperaturę do serwera 2\n");


        // Odbiór temperatury od serwera 2
        memset(recv_buffer, 0, sizeof(recv_buffer));
        recv_size2 = recvfrom(client_socket2, recv_buffer, sizeof(recv_buffer), 0, (struct sockaddr *)&server2_addr, &addr_len);
        if(recv_size2 < 0) {
            if (errno == EWOULDBLOCK) {
                printf("recvfrom timeout\n");
            } else {
                perror("recvfrom error");
            }
            exit(EXIT_FAILURE);
        }
        temp = recv_buffer[1];
        printf("Odebrano temperaturę od serwera 2: %d\n", temp);


        // Wysłanie wiadomości do serwera 1 o stanie temperatury
        memset(recv_buffer, 0, sizeof(recv_buffer));
        recv_buffer[1] = '0';
        if(temp > 30) {
            recv_buffer[0] = '1';
        }
        else {
            recv_buffer[0] = '0';
        }
        sent_size = sendto(client_socket1, recv_buffer, sizeof(recv_buffer), 0, (struct sockaddr *)&server1_addr, addr_len);
        if(sent_size < 0) {
            perror("sendto error");
            exit(EXIT_FAILURE);
        }
        printf("Wysłano wiadomość do serwera 1 o stanie temperatury\n");


        usleep(1750000);
    }

    // Zamknięcie gniazd
    if(close(client_socket1) < 0 || close(client_socket2) < 0) {
        perror("close error");
        exit(EXIT_FAILURE);
    }

    return 0;
}
