#include "net.h"

int set_server(struct server_info *s_info)
{
	struct sockaddr_in server_ipv4;
	struct sockaddr_in6 server_ipv6;
	struct in_addr ipv4;
	struct in6_addr ipv6;
	int test = 0;
	int exit_code = 0;
	int ipv4_ipv6;

	memset(&server_ipv6, 0, sizeof(server_ipv6));

	/* test and set ip address */
	if (s_info->all_ip)
	{
		ipv4_ipv6 = 0;
		server_ipv6.sin6_family = AF_INET6;
		server_ipv6.sin6_addr = in6addr_any;
		server_ipv6.sin6_port = htons(s_info->port);
	}
	else{
		test = inet_aton(s_info->address, &ipv4);
		if (test == 0){
			test = inet_pton(AF_INET6, s_info->address, &ipv6);
			if (!test || errno){
				fprintf(stderr, "IP address error: %s\n", s_info->address);
				exit(EXIT_FAILURE);
			}
			else{
				ipv4_ipv6 = 6;
				server_ipv6.sin6_addr = ipv6;
			}
		}
		else{
			if (test < 0){
				fprintf(stderr, "IP address error: %s\n", s_info->address);
				exit(EXIT_FAILURE);
			}
			ipv4_ipv6 = 4;
			server_ipv4.sin_addr.s_addr = ipv4.s_addr;
		}
	}
	/* according ip address type, set server_ip for socket */
	if (ipv4_ipv6 == 4){
		server_ipv4.sin_family = AF_INET;
		server_ipv4.sin_port = htons(s_info->port);
	}
	if (ipv4_ipv6 == 6){
		server_ipv6.sin6_family = AF_INET6;
		server_ipv6.sin6_port = htons(s_info->port);
	}
	/* start server */
	exit_code = StartServer(server_ipv4, server_ipv6, s_info, ipv4_ipv6);
	return exit_code;
}


int StartServer(struct sockaddr_in server_ipv4, struct sockaddr_in6 server_ipv6, struct server_info *s_info, int ipv4_ipv6)
{
	socklen_t length;
	int sock_server;
	int sock_client;
	int ret = 0;
	int on = 1;
	const char *ip_ret = NULL;
	char buf[BUFSIZE];
	struct sockaddr_in client_ipv4;
	struct sockaddr_in6 client_ipv6;
	char client_ip[CLIENT_IP];

	/* initialize socket */
	if (ipv4_ipv6==4)
		sock_server = socket(AF_INET, SOCK_STREAM, 0);
	else
		sock_server = socket(AF_INET6, SOCK_STREAM, 0);
	if (sock_server < 0){
		perror("create socket error");
		exit(EXIT_FAILURE);
	}
	if (setsockopt(sock_server, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0)
	{
		perror("setsockopt");
		exit(EXIT_FAILURE);
	}

	/* bind: if ip address is ipv6 or not set, bind ipv6 to accept ipv4 and ipv6 */
	if (ipv4_ipv6 == 4)
		ret = bind(sock_server, (struct sockaddr *) &server_ipv4, sizeof(server_ipv4));
	else
		ret = bind(sock_server, (struct sockaddr *) &server_ipv6, sizeof(server_ipv6));
	if (ret < 0){
		perror("bind error");
		exit(EXIT_FAILURE);
	}
	/* listen */
	listen(sock_server, 1);
	printf("listening....\n");
	do{
		if (ipv4_ipv6){
			length = sizeof(client_ipv4);
			sock_client = accept(sock_server, (struct sockaddr *)&client_ipv4, &length);
		}
		else{
			length = sizeof(client_ipv6);
			sock_client = accept(sock_server, (struct sockaddr *)&client_ipv6, &length);
		}
		if (sock_client == -1)
			perror("accept error");
		else{
			ret = read(sock_client, buf, BUFSIZE);
			printf("buf\n%s\n", buf);
			ret = request(buf);
			close(sock_client);
			break;
		}
	} while (TRUE);

	return 0;
}
