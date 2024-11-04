#include <sys/types.h>
#include <ifaddrs.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <errno.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

#define PORT "3434"
#define BACKLOG 10
#define MAXDATASIZE 500


int sockfd, new_fd, new_fds[BACKLOG]; //listen on sock_fd, new connections on new_fd	
int fd_listen[BACKLOG] = {[0 ... BACKLOG-1] = -2};
int fd_send[BACKLOG] = {[0 ... BACKLOG-1] = -2};
int fd_num = 0;
struct addrinfo hints, *servinfo, *p;
struct sockaddr_storage their_addr;

socklen_t sin_size;
struct sigaction sa;
int yes=1;
char s[INET6_ADDRSTRLEN];
int rv;

void say_hello(){
	printf("Hello world!!\n");
}

void exit_program(){
	exit(0);
}

void *get_in_addr(struct sockaddr *sa){
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

void show_my_ip(){
	struct ifaddrs *ifaddr, *ifa;
	int family;
	char address[200] = "";
	
	if (getifaddrs(&ifaddr) == -1){
		perror("getifaddrs error");	
		exit(1);
	}

	for (ifa = ifaddr; ifa != NULL; ifa = ifa -> ifa_next){
		if (ifa -> ifa_addr == NULL){
			continue;
		}

		inet_ntop(ifa->ifa_addr->sa_family, get_in_addr((struct sockaddr *)ifa->ifa_addr), address, sizeof address);
		if (strcmp(ifa->ifa_name, "wlo1") || !strcmp(address, "")){
			continue;
		}
		printf("%s: %s\n", (ifa->ifa_addr->sa_family == AF_INET)?("IPv4"):("IPv6"), address);
	}
	
	freeifaddrs(ifaddr);
}

void sigchld_handler(int s){
	(void)s;

	int saved_errno = errno;

	while(waitpid(-1, NULL, WNOHANG) > 0);

	errno = saved_errno;
}

int server_setup(){

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE; //use my IP
	
	if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}

	for (p=servinfo; p!=NULL; p->ai_next){
		if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
			perror("server: socket");
			continue;
		}

		if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
			perror("setsockopt");
			exit(1);
		}

		if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			close(sockfd);
			perror("server: bind");
			continue;
		}

		break;
	}

	freeaddrinfo(servinfo);

	if (p == NULL) {
		fprintf(stderr, "server: failed to bind\n");
		exit(1);
	}

	if (listen(sockfd, BACKLOG) == -1) {
		perror("listen");
		exit(1);
	}

	sa.sa_handler = sigchld_handler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	if (sigaction(SIGCHLD, &sa, NULL) == -1) {
		perror("sigaction");
		exit(1);
	}

	printf("server: waiting for connections...\n");

}

int server_listen(){
	char buf[MAXDATASIZE];
	while(1) {
		sin_size = sizeof their_addr;
		new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
		if (new_fd == -1) {
			perror("accept");
			continue;
		}
		inet_ntop(their_addr.ss_family, get_in_addr((struct sockaddr *)&their_addr), s, sizeof s);
		printf("server: got connection from %s\n", s);
		if (fd_num < BACKLOG){
			for (int i = 0; i < BACKLOG; ++i){
				if (fd_listen[i] == -2 && fd_send[i] == -2){
					printf("pre_listen\n");
					fd_listen[i] = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
					sleep(2);
					printf("pre_send\n");
		//			fd_send[i] = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
		//			printf("after_accept\n");
		//			fd_num++;
					break;
				}
			}
		} else {
			perror("BACKLOG full");
		}	
		if (!fork()) {
			close(sockfd);
			if (send(new_fd, "Hello there!", 13, 0) == -1)
				perror("send");
			if (recv(new_fd, buf, MAXDATASIZE-1, 0) == -1)
				perror("recv");
			printf("%s\n", buf);
			close(new_fd);
			exit(0);
		}
		close(new_fd);
	}

	return 0;
}

void server(){
	server_setup();
	server_listen();
}
