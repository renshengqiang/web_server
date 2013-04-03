#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>

int main()
{
	int sock_fd1, sock_fd2;
	int len;
	struct sockaddr_in address;
	int result;
	char ch;

	sock_fd1 = socket(AF_INET,SOCK_STREAM,0);
	sock_fd2 = socket(AF_INET,SOCK_STREAM,0);
	address.sin_family = AF_INET;
	address.sin_port = htons(6000);
	address.sin_addr.s_addr = inet_addr("127.0.0.1");
	len = sizeof(address);
	
	result = connect(sock_fd1,(struct sockaddr *)&address,len);
    printf("sock1 %d\n", result);
	result = connect(sock_fd2,(struct sockaddr *)&address,len);
    printf("sock1 %d\n", result);

    write(sock_fd1, "hello", 5);
    write(sock_fd2, "world", 5);
	if(-1 == result)
	{
		perror("oops:client1\n");
		exit(1);
	}
	read(sock_fd1,&ch,1);
	ch++;
	write(sock_fd1,&ch,1);
	printf("char from server = %c\n",ch);
	close(sock_fd1);
	exit(0);
}
