#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "server.h"
#include "utilities.h"

void load_config(struct config_t *config)
{
	FILE *fp;
	unsigned int port;
	char buf[1024];

	fp = open_file(CONFIG_FILE, "r");
	fgets(buf, 1024, fp);
	if (search_string(buf, "port") >= 0)
		sscanf(buf + strlen("port: "), "%ud", &port);
	config->port = port;
	fclose(fp);
}

void run_server(struct config_t *config)
{
	int serv_port = config->port;
	int listenq = 10;
	int listenfd, connfd;
	pid_t childpid;
	socklen_t clilen;
	struct sockaddr_in cliaddr, servaddr;

	if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		fprintf(stderr, "socket error\n");
		exit(-1);
	}

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(serv_port);

	if (bind(listenfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0) {
		fprintf(stderr, "bind error\n");
		exit(-1);
	}

	if (listen(listenfd, listenq) < 0) {
		fprintf(stderr, "listen error\n");
		exit(-1);
	}
	printf("listening...\n");

	for (; ;) {
		clilen = sizeof(cliaddr);
		if ((connfd = accept(listenfd, (struct sockaddr *) &cliaddr, &clilen)) < 0) {
		    fprintf(stderr, "accept error\n");
            fflush(stdout);
			exit(-1);
		}
	//	usleep(100000);
		printf("pid: %d\taccepted\tconnfd = %d\n", getpid(), connfd);

		if ((childpid = fork()) == 0) {
			printf("pid: %d\tentering\n", getpid());
            fflush(stdout);
			close(listenfd);
			str_echo(connfd);
			printf("pid: %d\texiting\n", getpid());
            fflush(stdout);
			exit(0);
		} else if (childpid > 0) {
			printf("pid: %d\tparent\n", getpid());
            fflush(stdout);
			close(connfd);
		} else {
			fprintf(stderr, "fork error\n");
			exit(-1);
		}
	}
}

void str_echo(int sockfd)
{
	ssize_t n;
	char buf[2048];
	char *send_str = "HTTP/1.1 200 OK\r\n"
				//	 "Date: Wed, 03 Apr 2013 00:56:58 GMT\r\n"
					 "Server: Apache/2.0.52 (Red Hat)\r\n"
				//	 "Transfer-Encoding: chunked\r\n"
					 "Content-Length: 35\r\n"
					 "Content-Type: text/html\r\n\r\n"
					 "<html>\n\tthis is a html test\n</html>";

	printf("pid: %d\tin str_echo\n", getpid());

again:
	if ((n = read(sockfd, buf, 2048)) > 0) {
		write(STDOUT_FILENO, buf, n);
		printf("pid: %d\tsend:\n%s\n", getpid(), send_str);
        fflush(stdout);
		write(sockfd, send_str, strlen(send_str));	
		close(sockfd);
	} else if (n < 0 && errno == EINTR) {
		printf("\nEINTR\n");
		goto again;
	} else if (n < 0) {
		fprintf(stderr, "str_echo: read error\n");
		exit(-1);
	}
}

