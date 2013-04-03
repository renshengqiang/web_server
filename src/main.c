#include <stdlib.h>
#include <stdio.h>

#include "server.h"

int main()
{
	struct config_t config;

	load_config(&config);
	run_server(&config);
	return 0;
}

