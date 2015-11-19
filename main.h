#ifndef _MAIN_H
#define _MAIN_H


#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <bsd/stdlib.h>
#include <netinet/in.h>

#include <ctype.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

struct server_info
{
	int debug_mode;
	unsigned short port;
	int all_ip;
    char *address;
	char *cgi_dir;
    char *server_root;
	char *logging_file;
};

struct server_info initial_info(struct server_info s_info);
void usage(int i);

#endif 
