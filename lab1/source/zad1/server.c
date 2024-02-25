#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <errno.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>

#define MAX_BUF 128
#define PORT "3002"

int s, new_s;
struct addrinfo h;
struct addrinfo *r=NULL, *p; 
unsigned char mip_str[INET_ADDRSTRLEN];

int main()
{

    memset(&h, 0, sizeof(struct addrinfo));

    h.ai_family=PF_INET;
    h.ai_socktype=SOCK_STREAM;
    h.ai_flags=AI_PASSIVE;

    if(getaddrinfo(NULL, PORT, &h, &r)!=0){ 
        printf("Error: %s (%s:%d)\n", strerror(errno), __FILE__, __LINE__);
        exit(-1);
    }

    s=socket(r->ai_family, r->ai_socktype, r->ai_protocol);

    if(s==-1){
        printf("Error: %s (%s:%d)\n", strerror(errno), __FILE__, __LINE__);
        exit(-1);
    }

    if(bind(s, r->ai_addr, r->ai_addrlen)!=0){
        printf("Error: %s (%s:%d)\n", strerror(errno), __FILE__, __LINE__);
        exit(-1);
    }

    if(listen(s, 1)!=0){
        printf("Error: %s (%s:%d)\n", strerror(errno), __FILE__, __LINE__);
        exit(-1);
    }

    struct sockaddr_in their_addr;
    socklen_t addr_size=sizeof(their_addr);

    if((new_s=accept(s, (struct sockaddr *)&their_addr, &addr_size))==-1){
        printf("Error: %s (%s:%d)\n", strerror(errno), __FILE__, __LINE__);
        exit(-1);
    }

    long i = 0;

    for(;;){
        i = i + 1;
        char message[MAX_BUF];
        int result=recv(new_s, message, MAX_BUF, 0);

        if(result==0){
            printf("Peer was disconeted\n");
            break;
        }
        else if(result<0){
            printf("ERROR: %s\n", strerror(errno));
            exit(-4);
        }

        message[result]='\0';
        size_t dataSize = strlen(message) + 1;

        struct timeval tv;
        gettimeofday(&tv, NULL);
        time_t now = tv.tv_sec;
        struct tm local_time;
        localtime_r(&now, &local_time);

        char time_str[100];
        strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", &local_time);

        char full_time_str[100];
        snprintf(full_time_str, sizeof(full_time_str), "%s.%03d", time_str, (int)tv.tv_usec / 1000);

        printf("S: %zuB", dataSize);
        printf(" N %d: %s", i, full_time_str);
        printf(" R: %s\n", message);
        
        
    }
    freeaddrinfo(r);
    close(new_s);
    close(s);
}