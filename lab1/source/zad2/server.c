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
#include <stdbool.h>

#define MAX_BUF 256
#define Client "192.168.56.101"
#define Client_PORT "9558"
#define HELLO_MESSAGE 'h'
#define NORMAL_MESSAGE 'm'
#define GOODBYE_MESSAGE 'g'

int main(int argc, char *argv[]){

	bool done = false;
	double uptime;
	if(argc <= 2){
		if(argc == 2){
			if(strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "-help") == 0){
				printf("Syntax: %s [uptime (0=infinity)]\n", argv[0]);
				exit(0);
			}else{
				uptime = atof(argv[1]);
			}
		}else{
			uptime = 0;
		}
	}else{
		printf("Syntax: %s [uptime (0=infinity)]\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	unsigned char send_m[MAX_BUF];
	unsigned char recv_m[MAX_BUF];
	int s; //socket
	int pos;
	struct addrinfo h, *r; //hints, res
	struct sockaddr_in c;
	int c_len = sizeof(c);
	char message_t;

	//uptime
	time_t start_time;
	double time_elapsed;
	time(&start_time);

	//select
	fd_set readfds;
	struct timeval tv;
	int res_select;
	tv.tv_sec=0;
	tv.tv_usec=5000;

	//set addr info
	memset(&h, 0, sizeof(struct addrinfo));
	h.ai_family = PF_INET;
	h.ai_socktype = SOCK_DGRAM;

	if(getaddrinfo(Client, Client_PORT, &h, &r) != 0){
		fprintf(stderr, "ERROR: Couldn't  get Client info (%s:%d)\n", __FILE__, __LINE__-1);
		exit(EXIT_FAILURE);
	}
	if(uptime == 0){
		printf("Oczekiwanie na pojawienie sie serwera\n");
	}else{
		printf("Planned uptime: %.1f seconds\n", uptime);
	}

	//create a socket
	s=socket(r->ai_family, r->ai_socktype, r->ai_protocol);
	if(s==-1){
		fprintf(stderr, "ERROR: %s (%s:%d)\n", strerror(errno), __FILE__, __LINE__-2);
		exit(EXIT_FAILURE);
	}

	//send Hello message
	message_t = HELLO_MESSAGE;
	snprintf(send_m, MAX_BUF, "%cHello!", message_t);
	printf("Sending HELLO message\n");
	pos = sendto(s, send_m, strlen(send_m), 0, r->ai_addr, r->ai_addrlen);
	if(pos < 0){
		fprintf(stderr, "ERROR: %s (%s,%d)\n", strerror(errno), __FILE__, __LINE__-2);
		exit(EXIT_FAILURE);
	}

	printf("Waiting for client messages...\n");
	while(!done){
		FD_ZERO(&readfds);
		FD_SET(s, &readfds);

		res_select = select(s+1, &readfds, NULL, NULL, &tv);
		if(r < 0){
			fprintf(stderr, "ERROR: %s (%s:%d)\n", strerror(errno), __FILE__, __LINE__-2);
			exit(EXIT_FAILURE);
		}else if(res_select > 0){
			if(FD_ISSET(s, &readfds)){
				//wiadomosc od klienta
				pos = recvfrom(s, recv_m, MAX_BUF, 0, (struct sockaddr*)&c, &c_len);
				if(pos < 0){
					fprintf(stderr, "ERROR: %s (%s,%d)\n", strerror(errno), __FILE__, __LINE__-2);
					exit(EXIT_FAILURE);
				}

				recv_m[pos]='\0';
				message_t = recv_m[0];
				snprintf(recv_m, MAX_BUF, "%s", recv_m+1);

				if(message_t == NORMAL_MESSAGE){
					//Received message
					printf("Received message: \"%s\"\n", recv_m);

					//Send response
					char date[20];
                    time_t now = time(NULL);
                    strftime(date, 20, "%Y-%m-%d %H:%M:%S", localtime(&now));
					int random_number = rand() % 100; // Generuj od 0 do 99
        			snprintf(send_m, MAX_BUF, "%c%d Date:%s", message_t, random_number, recv_m, date);
					printf("Sending message: \"%s\"\n", send_m+1);

					pos = sendto(s, send_m, strlen(send_m), 0, r->ai_addr, r->ai_addrlen);
					if(pos < 0){
						fprintf(stderr, "ERROR: %s (%s,%d)\n", strerror(errno), __FILE__, __LINE__-2);
						exit(EXIT_FAILURE);
					}
				}
			}
		}else{
			time_elapsed = difftime(time(NULL), start_time);
			if(time_elapsed >= uptime && uptime != 0){
				done = true;
			}
		}
	}

	printf("Quitting...\n");
	printf("Sending GOODBYE message\n");
	snprintf(send_m, MAX_BUF, "%cGoodbye!", GOODBYE_MESSAGE);

	pos = sendto(s, send_m, strlen(send_m), 0, r->ai_addr, r->ai_addrlen);
	if(pos < 0){
		fprintf(stderr, "ERROR: %s (%s,%d)\n", strerror(errno), __FILE__, __LINE__-2);
		exit(EXIT_FAILURE);
	}
	printf("Client has worked for %.1f seconds\n", time_elapsed);
	//clean up
	freeaddrinfo(r);
	close(s);

	return 0;
}