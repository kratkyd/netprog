#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <stdio.h>

#define PORT "3434"

int main(int argc, char *argv[]){
	struct addrinfo hints, *res;
	int sockfd;
	char *message[50];

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;	
	hints.ai_socktype = SOCK_STREAM;

	getaddrinfo("10.77.41.180", "3434", &hints, &res);

	sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	connect(sockfd, res->ai_addr, res->ai_addrlen);
	
	recv(sockfd, *message, sizeof(message), 0);
	
	printf("message is %s\n", &message);

	return 0;
}
