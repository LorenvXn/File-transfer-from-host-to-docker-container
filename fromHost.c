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

#define PORT "8666" 
#define LENGTH 512

void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(int argc, char *argv[])
{
    int sockfd, numbytes;  
//    char buf[MAXDATASIZE];
    struct addrinfo hints, *servinfo, *p;
    int receive;
    char s[INET6_ADDRSTRLEN];

    if (argc != 2) {
        fprintf(stderr,"usage: fromHost [hostname|IP]\n");
        exit(1);
    }

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if ((receive = getaddrinfo(argv[1], PORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(receive));
        return 1;
    }

    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            perror("client: socket");
            continue;
        }
        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("client: connect");
            continue;
        }

        break;
    }

    if (p == NULL) {
        fprintf(stderr, "container: failed to connect\n");
        return 2;
    }

    inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
            s, sizeof s);
    printf("container: connecting to host %s\n", s);

    
    freeaddrinfo(servinfo); 

    char receive_buf[LENGTH];
    char *fr_name = "/home/test/receive.txt";
    FILE *fr=fopen(fr_name, "a");
	if(fr==NULL)
		printf("file %s cannot be opened on container. \n", fr_name);
	else
	{
		memset(&receive_buf, 0, sizeof (receive_buf));
		int fr_block_sz=0;
		while((fr_block_sz = recv(sockfd, receive_buf, LENGTH, 0)) >0 )
		{
	 	int write_sz=fwrite(receive_buf, sizeof(char), fr_block_sz, fr);
	 	if(write_sz<fr_block_sz)
	   		{
			error("file write failed on client.\n");
	   		}
			memset(&receive_buf, 0, sizeof (receive_buf));
			 if (fr_block_sz == 0 || fr_block_sz != 512) 
               		 {
               		     break;
               		 }
		}

	  if(fr_block_sz < 0)
            {
                if (errno == EAGAIN)
                {
                    printf("recv() timed out.\n");
                }
                else
                {
                    fprintf(stderr, "recv() failed due to errno = %d\n", errno);
                    exit(1);
                }
            }

            printf("File received !\n");
            fclose(fr); 
	
	}
 	   close(sockfd);

	    return 0;
}
