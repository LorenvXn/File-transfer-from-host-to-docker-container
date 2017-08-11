#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>

#define PORT "8666"  //use same port
#define LENGTH 512 
#define BACKLOG 10

void sigchld_handler(int s)
{
	int saved_errno = errno;

	while(waitpid(-1, NULL, WNOHANG) > 0);

	errno = saved_errno;
}


void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(void)
{
	int sockfd, new_fd;  
	struct addrinfo hints, *servinfo, *p;
	struct sockaddr_storage _address; 
	socklen_t sin_size;
	struct sigaction sa;
	int yes=1;
	char s[INET6_ADDRSTRLEN];
	int rv;

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE; 

	if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	  }
    
 	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype,
				p->ai_protocol)) == -1) {
			perror("host: socket");
			continue;
		}

		if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
				sizeof(int)) == -1) {
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

	if (p == NULL)  {
		fprintf(stderr, "host: failed to bind\n");
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

	while(1) {  
		sin_size = sizeof _address;
		new_fd = accept(sockfd, (struct sockaddr *)&_address, &sin_size);
		if (new_fd == -1) {
			perror("accept");
			continue;
		}

		inet_ntop(_address.ss_family,
			get_in_addr((struct sockaddr *)&_address),
			s, sizeof s);
		printf("host: got connection from %s\n", s);

		    char* file_to_transfer = "/home/tron/chichi/send.txt";
		    char sendbuff[LENGTH]; /
		    printf("[host] Sending %s to the container...", file_to_transfer);
		    FILE *fs = fopen(file_to_transfer, "r");
		    if(fs == NULL)
		    {
		        fprintf(stderr, "ERROR: File %s not found on host . (errno = %d)\n", file_to_transfer, errno);
				exit(1);
		    }

		    bzero(sendbuff, LENGTH); 
		//	memset(&sendbuff, 0, sizeof(sendbuff))
		    int file_size; 
		    while((file_size = fread(sendbuff, sizeof(char), LENGTH, fs))>0)
		    {
		        if(send(new_fd, sendbuff, file_size, 0) < 0)
		        {
		            fprintf(stderr, "ERROR: failed to send file %s to container (errno = %d)\n", file_to_transfer, errno);
		            exit(1);
		        }
		        bzero(sendbuff, LENGTH);
		    }
		    printf("File sent!................\n");
		    close(new_fd);
		    printf("[Host] Connection with container closed. Host will wait now...\n");
		    while(waitpid(-1, NULL, WNOHANG) > 0);

	}

	return 0;
}


