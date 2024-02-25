#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <time.h>
#include <sys/time.h>

#define SERVER "192.168.56.101"
#define PORT "3002"
#define MAXDATASIZE 128

int sockfd, numbytes;
char buf[MAXDATASIZE];
struct addrinfo hints, *res, *p;
int rv;
unsigned char mip_str[INET_ADDRSTRLEN];

int main(int argc, char *argv[]) {

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    if ((rv = getaddrinfo(SERVER, PORT, &hints, &res)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    for(p = res; p != NULL; p = p->ai_next) {
        if((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1){
            printf("%d\n", sockfd);
            perror("client: socket");
            continue;
        }
        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("client:connect");
            continue;
        }
        break;
    }
    
    if (p == NULL) {
        fprintf(stderr, "client:failed to connect\n");
        return 2;
    }

    struct sockaddr_in *mip=(struct sockaddr_in*)p->ai_addr;
    inet_ntop(p->ai_family,  &(mip->sin_addr), mip_str, INET_ADDRSTRLEN);

    printf("client: connecting to %s\n", mip_str);

    freeaddrinfo(res);

    long i = 1;
    for(;;){
        char str[64];
        time_t t;
        time(&t);
        sprintf(str, "%d", i);
        strcat(str, ". ");
        strcat(str, ctime(&t));

        struct timeval tv;
        gettimeofday(&tv, NULL);

        time_t now = tv.tv_sec;
        struct tm local_time;
        localtime_r(&now, &local_time);

        // Formatowanie daty i czasu do odpowiedniego formatu
        char time_str[100]; // Bufor na sformatowaną datę i czas
        strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", &local_time);

        // Dodanie milisekund do sformatowanej daty
        char full_time_str[100]; // Bufor na pełną datę i czas z milisekundami
        snprintf(full_time_str, sizeof(full_time_str), "%s.%03d", time_str, (int)tv.tv_usec / 1000);
        
        i=i+1;

        printf(full_time_str);

        if(send(sockfd, full_time_str, strlen(full_time_str), 0) == -1) {
            perror("send");
        }
        
        usleep(1400000);
    }
    close(sockfd);
}