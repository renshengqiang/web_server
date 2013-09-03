#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <dirent.h>
#include <signal.h>
#include <sys/wait.h>
#include "server.h"
#include "utilities.h"

void sigchild_handler(int n)
{
    int pid = waitpid(-1, NULL, WNOHANG); //DONT HANG UP
    printf("child %d exit\n", pid);
    return;
}
void load_config(struct config_t *config)
{
	FILE *fp;
	unsigned int port;
	char buf[1024];

	fp = open_file(CONFIG_FILE, "r");
	fgets(buf, 1024, fp);
	if (search_string(buf, "port") >= 0)
		sscanf(buf + strlen("port: "), "%u", &port);
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
    int reusable = 1;

	if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		fprintf(stderr, "socket error\n");
		exit(-1);
	}

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(serv_port);

    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, (const char *)&reusable, sizeof(int));
	if (bind(listenfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0) {
		fprintf(stderr, "bind error\n");
		exit(-1);
	}

	if (listen(listenfd, listenq) < 0) {
		fprintf(stderr, "listen error\n");
		exit(-1);
	}
	printf("listening...\n");

    signal(SIGCHLD, sigchild_handler);
	for (; ;) {
		clilen = sizeof(cliaddr);
		if ((connfd = accept(listenfd, (struct sockaddr *) &cliaddr, &clilen)) < 0) {
		    fprintf(stderr, "accept error\n");
            fflush(stdout);
			exit(-1);
		}
		printf("pid: %d\taccepted\tconnfd = %d\n", getpid(), connfd);

		if ((childpid = fork()) == 0) {
			printf("pid: %d\tentering\n", getpid());
            fflush(stdout);
			close(listenfd);

            dir_echo(connfd);
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

void read_dir(char *buf, char *path)
{
    char *head_str = "HTTP/1.1 200 OK\r\n"
                     "Server: Gaoyuan/0.01 (Ubuntu 12.04)\r\n"
                     "Content-Type: text/html\r\n\r\n"
                     "<html>\n\t";
    DIR             *dp;
    struct dirent   *dirp;
    char dir_item[1024];

    strcpy(buf, head_str);
    if( (dp = opendir(path)) != NULL)
    {
        while( (dirp = readdir(dp)) != NULL)
        {
            if(!is_end_with_dot(dirp->d_name))
            {
                if(strcmp(path, "/") == 0)
                {
                    strcpy(path, "/.");
                }

                sprintf(dir_item, "<p><a href=\"%s/%s\">", path, dirp->d_name); 
                strcat(buf, dir_item);
                sprintf(dir_item, "\"%s/%s\"", path, dirp->d_name); 
                strcat(buf, dir_item );
                strcat(buf, "</a></p>");
            }
        }
        closedir(dp);
    }
    strcat(buf, "</html>");
}
void read_file(char *buf, char *fname)
{
    char *head_str = "HTTP/1.1 200 OK\r\n"
                     "Server: Gaoyuan/0.01 (Ubuntu 12.04)\r\n"
                     "Content-Type: text/html\r\n\r\n"
                     "<html>\n\t";
    FILE *fp = open_file(fname ,"r");
    char file[20480];

    strcpy(buf, head_str);
    fread(file, 20480-strlen(head_str)-7, 1, fp);
    strcat(buf, file);
    strcat(buf, "</html>");
    fclose(fp);
}

void fill_error(char *buf, char *fname, int status)
{
    char *head_str = "HTTP/1.1 200 OK\r\n"
                     "Server: Gaoyuan/0.01 (Ubuntu 12.04)\r\n"
                     "Content-Type: text/html\r\n\r\n";
    char info[1024];

    sprintf(info, "<html>\n\topen file %s error, status %d</html>", fname, status);
    strcpy(buf, head_str);
    strcat(buf, info);
}

void dir_echo(int sockfd)
{
    ssize_t n;
    char recv_buf[2048];
    char send_buf[20480];
    char dir[1024];
    int ret;

again:
    if((n = read(sockfd, recv_buf, 2048))> 0)
    {
        printf("Recved Request:\n");
        write(STDOUT_FILENO, recv_buf, n);
        
        sscanf(recv_buf, "GET %s", dir);
        printf("client request for:%s\n", dir);

        ret = dir_or_file(dir);
        if(ret == 1)
        {
            read_dir(send_buf, dir);
        }
        else if(ret == 2)
        {
            read_file(send_buf, dir);
        }
        else
        {
            fill_error(send_buf, dir, ret);
        }
        write(sockfd, send_buf, strlen(send_buf));
        close(sockfd);
    }
    else if(n < 0 && errno == EINTR)
    {
        printf("EINTR\n");
        goto again;
    }
    else
        printf("Read request from client error\n");
}
