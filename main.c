#include "main.h"
#include "net.h"

int main(int argc, char **argv) 
{
    int op;
    struct server_info s_info;
    char *path;
    char *address;
    char *file;
    char *port; /* default 8080 */
    char *root;
    int exit_code = 0;
	int i = 0;
	int test_digit = 1;

    setprogname(argv[0]);
    
    s_info = initial_info(s_info);
    while ((op = getopt(argc, argv, "dhc:i:l:p:")) != -1) {
        switch (op) {
        case 'd':
            s_info.debug_mode = 1;
            break;
        case 'h':
            /* print usage and exit */
            usage(0);
            break;
        case 'c':
            path = optarg;
            s_info.cgi_dir = path;
            break;
        case 'i':
			s_info.all_ip = 0;
            address = optarg;
            s_info.address = address;
            break;
        case 'l':
            file = optarg;
            s_info.logging_file = file;
            break;
        case 'p':
			port = optarg;
			for (i = 0; i < strlen(port); i++){
				isdigit(port[i]) > 0 ? (test_digit &= 1) : (test_digit &= 0);
			}
			if (test_digit)
				s_info.port = atoi(port);
			else{
				fprintf(stderr, "port format error:%s\n", port);
				exit(EXIT_FAILURE);
			}
			if (atoi(port) > 65535 || atoi(port) < 0){
				fprintf(stderr, "port:%s overranged\n", port);
				exit(EXIT_FAILURE);
			}
            break;
        case '?':
            usage(1);
            break;
        case ':':
            usage(2);
            break;
        default:
            usage(3);
            break;
        }
    }
    argc -= optind;
    argv += optind;
    root = argv[0];
    if (argc)
        s_info.server_root = root;
    else {
        printf("please set root dir for server\n");
        usage(4);
        return 1;
    }
    exit_code = set_server(&s_info);
    return exit_code;
}

void usage(int i)
{
    printf("%d\n", i);
    switch (i) {
    case 0:
        printf("usage:\n\n");
        printf("SYNOPSIS\n");
        printf("sws [-dh][-c dir][-i address][-l file][-p port] dir\n\n");
        printf("OPTIONS\n");
        printf("-c dir: 	Allow execution of CGIs from the given directory\n");
        printf("-d: 		Enter debugging mode\n");
        printf("-i address:	Bind to the given IPv4 or IPv6 address\n");
        printf("-l file:	Log all requests to the given file\n");
        printf("-p port:	Listen on the given port\n");
        printf("\n");
        break;
	case 1:
	case 2:
		printf("SYNOPSIS\n");
		printf("sws [-dh][-c dir][-i address][-l file][-p port] dir\n\n");
		break;
	case 3:
		printf("unknown error\n");
		break;
    case 4:
        printf("usage:\n");
        printf("sws [-dh][-c dir][-i address][-l file][-p port] dir\n\n");
        break;
    }
}

struct server_info initial_info(struct server_info s_info) 
{
    s_info.debug_mode = 0;
    s_info.port = 8080;
    s_info.address = NULL;
    s_info.server_root = NULL;
    s_info.cgi_dir = NULL;
    s_info.logging_file = NULL;
	s_info.all_ip = 1;
    return s_info;
}
