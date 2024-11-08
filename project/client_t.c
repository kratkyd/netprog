/*
** client.c -- a stream socket client demo
*/

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

#define PORT "3434" // the port client will be connecting to 

#define MAXDATASIZE 100 // max number of bytes we can get at once 

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(int argc, char *argv[])
{
    int sockfd, newfd, numbytes;  
    int fd_listen, fd_send;
    char buf[MAXDATASIZE];
    struct addrinfo hints, *servinfo, *p;
    int rv;
    char s[INET6_ADDRSTRLEN];

    if (argc != 2) {
        fprintf(stderr,"usage: client hostname\n");
        exit(1);
    }

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if ((rv = getaddrinfo(argv[1], PORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and connect to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((newfd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            perror("client: socket");
            continue;
        }

        if (connect(newfd, p->ai_addr, p->ai_addrlen) == -1) {
        	close(newfd);
        	perror("client: connect");
        	continue;
        }
	usleep(50);


	
	if ((fd_send = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
		close(newfd);
		perror("client: socket - send");
		continue;
	}

	if (connect(fd_send, p->ai_addr, p->ai_addrlen) == -1) {
		close(newfd);
		close(fd_send);
		perror("client: connect - send");
		continue;
	}

	
	usleep(50);

	if ((fd_listen = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1){
		close(newfd);
		close(fd_send);
		perror("client: socket - listen");
		continue;
	}

	if (connect(fd_listen, p->ai_addr, p->ai_addrlen) == -1) {
		close(newfd);
		close(fd_send);
		close(fd_listen);
		perror("client: listen");
		continue;
	}	
	printf("listen socket established\n");

        break;
    }

    if (p == NULL) {
        fprintf(stderr, "client: failed to connect\n");
        return 2;
    }

    inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
            s, sizeof s);
    printf("client: connecting to %s\n", s);

    freeaddrinfo(servinfo); // all done with this structure

    if ((numbytes = recv(newfd, buf, MAXDATASIZE-1, 0)) == -1) {
        perror("recv");
        exit(1);
    }
    

    buf[numbytes] = '\0';

    printf("client: received '%s'\n",buf);

// 	if (send(newfd, "Hello right back!", 18, 0) == -1)
//	   perror("send");
//
//	if (send(fd_send, "Another message!", 17, 0) == -1)
//		perror("send");
//
//	if (send(fd_send, "Yet another message!", 21, 0) == -1)
//		perror("send");
//		
	while(1){
		scanf("%s", &buf);
		if (send(fd_send, buf, MAXDATASIZE, 0) == -1)
			perror("send error");		
	}
	
    close(newfd);
    return 0;
}
