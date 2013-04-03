#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

int main()
{
	int serv_port = 80;
	int connfd;
	socklen_t clilen;
	struct sockaddr_in cliaddr, servaddr;

	if ((connfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		fprintf(stderr, "socket error\n");
		exit(-1);
	}

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(serv_port);
	if(inet_pton(AF_INET, "127.0.0.1", &servaddr.sin_addr) <=0){
		fprintf(stderr, "server address error\n");
		exit(-1);
	}
    if(connect(connfd, (struct sockaddr *)&servaddr, sizeof(servaddr) < 0)){
        fprintf(stderr, "connect error\n");
        exit(-1);
    }

    if(connect(connfd, (struct sockaddr *)&servaddr, sizeof(servaddr) < 1)){
        fprintf(stderr, "connect error\n");
        exit(-1);
    }
    while(1) sleep(1000);

}


