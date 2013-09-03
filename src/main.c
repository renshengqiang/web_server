#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "server.h"

int main()
{
	struct config_t config;

    if(fork() == 0)
    {
        setsid();
	    load_config(&config);
	    run_server(&config);
    }
	return 0;
}

