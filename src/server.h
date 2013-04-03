#ifndef SERVER_H_
#define SERVER_H_

#define CONFIG_FILE "config"
#define MAXLINE 80

void load_config();

void run_server();

void str_echo(int sockfd);

struct config_t {
	unsigned int port;
};

#endif

