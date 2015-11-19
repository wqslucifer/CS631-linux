#ifndef _NET_H
#define _NET_H

#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <net/if.h>

#include <netinet/in.h>

#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <ifaddrs.h>

#include "main.h"
#include "http.h"

#define BUFSIZE 2048
#define CREATE_SOCK_ERR 1
#define BIND_ERR 2
#define TRUE 1
#define CLIENT_IP 40

int set_server(struct server_info *s_info);
int StartServer(struct sockaddr_in server_ipv4, struct sockaddr_in6 server_ipv6, struct server_info *s_info, int ipv4_ipv6);

#endif
