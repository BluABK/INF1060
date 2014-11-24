#include <stdio.h>
#include <stdlib.h>
#include <string.h>		// memset
#include <unistd.h>		// close
#include <netdb.h>		// gai_strerror

#include <sys/types.h>		// getaddrinfo, freeaddrinfo, gai_strerror
#include <sys/socket.h>		// getaddrinfo, freeaddrinfo, gai_strerror
#include <netinet/in.h>
#include <arpa/inet.h>		// inet_ntop
#include <netdb.h>		// getaddrinfo, freeaddrinfo, gai_strerror

// reads a line up to len-1 bytes w/newline
int readline(int fd, char *buf, size_t len) {
	size_t pos = 0;
	char rbuf;
	int retv;
	while(pos < len-1) {
		retv = read(fd, &rbuf, 1);
		if (retv <= 0) {
			// huge error
			perror("readline");
			return -1;
		}
		if (rbuf == '\n') {
			break;
		} else {
			buf[pos++] = rbuf;
		}
	}
	buf[pos] = 0;
	/* 0 - infiniteish */
	return pos;

}

void help() {
	fprintf(stderr, "ls   - list current directory\n");
	fprintf(stderr, "pwd  - get working directory\n");
	fprintf(stderr, "cd   - change working directory\n");
	fprintf(stderr, "get  - get and write file to standard output\n");
	fprintf(stderr, "stat - get file information\n");
	fprintf(stderr, "help\n");
	fprintf(stderr, "quit\n");
}

void prompt(const char *rpath) {
	fprintf(stdout, "[%s]>: ", rpath);
	fflush(stdout);
}

int main(int argc, char* argv[]) {

	int retv = 0;
	if (argc < 3) {
		printf("Usage: %s [host] [port]\n", argv[0]);
		return 0;
	}

	// getaddrinfo
	const char *hostname = argv[1];
	char *port = argv[2];
	struct addrinfo hints;
	struct addrinfo *res;
	char input[256];

	memset(&hints, 0, sizeof(hints));

	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = 0;

	retv = getaddrinfo(hostname, port, &hints, &res);

	if (retv != 0) {
		printf("getaddrinfo failed: %s\n", gai_strerror(retv));
		return -1;
	}

	int fd = socket(AF_INET, SOCK_STREAM, 0);
	if (fd == -1) {
		perror("socket");
		return -2;
	}

	struct addrinfo *cur = res;
	while (cur) {
		if (cur->ai_family != AF_INET) {
			cur = cur->ai_next;
			continue;
		}

		// Connect to remote host
		retv = connect(fd, cur->ai_addr, cur->ai_addrlen);
		if(retv >= 0) {
			struct sockaddr_in *cur_addr = (struct sockaddr_in*)cur->ai_addr;
			char buf[256];
			buf[0] = 0;

			inet_ntop(AF_INET, &(cur_addr->sin_addr.s_addr), buf, sizeof(buf));

			printf("Connecting to remote host: %s:%s\n", buf, port);
			break;
		}
		cur = cur->ai_next;
	}
	freeaddrinfo(res);
	if(retv < 0) {
		perror("connect");
		close(fd);
		return -1;
	}

	// main loop
	help();
	//	while(1) {
	while(prompt(hostname), fgets((void *)input, sizeof(input), stdin)) {

		char *tok = strtok(input, " \n");
		char *arg = strtok(NULL, "\n");

		if (arg != NULL) {
			while (*arg && *arg == ' ')
				arg++;
			if (!*arg)
				arg = NULL;
		}

		if (!tok) {
			continue;
		} else if (!strcmp(tok, "help")) {
			help();
		} else if (!strcmp(tok, "ls")) {
			write(fd, "ls\n", 3);
			
			char sresp[512];
			int ret;
			
			while((ret = readline(fd, sresp, sizeof(sresp))) > 0) {
				printf("%s\n", sresp);
			}
			if (ret < 0) {
				break;
			}
		} else if (!strcmp(tok, "pwd")) {
			// Send command to server
			send(fd, "pwd\n", 4, 0);
			
			// Read response char-by-char, and end packet by char '\n'
			char sresp[512];

			if (readline(fd, sresp, sizeof(sresp)) <= 0) {
				break;
			}

			printf("Current directory: %s\n", sresp);
		} else if (!strcmp(tok, "cd")) {
			printf("Not implemented\n");
			// dummy
		} else if (!strcmp(tok, "get")) {
			if (!arg) {
				printf("I hate you\n");
				continue;
			}
			send(fd, "get\n", 4, 0);
			send(fd, arg, strlen(arg), 0);
			send(fd, "\n", 1, 0);

			char sresp[512];
			int ret = readline(fd, sresp, sizeof(sresp));
			if (ret < 0) {
				break;
			}
			if (!strcmp(sresp, "error")) {
				printf("Server refused to give me the file\n");
				continue;
			}
			size_t len;
			// Sending decimals over a text protocol makes hton/ntoh irrelevant
			sscanf(sresp, "%lu", &len);
			printf("Reading %lu bytes\n", len);

			// len a.k.a. remaining
			while(len > 0) {
				size_t toread = sizeof(sresp);
				if (len < toread) {
					toread = len;
				}

				retv = read(fd, sresp, toread);
				if (retv <= 0) {
					perror("read");
					break;
				}
				int i;
				for (i = 0; i < retv; i++) {
					if (sresp[i] >= 32 && sresp[i] <= 126) continue;
					if (sresp[i] == 10 || sresp[i] == 13) continue;
					sresp[i] = '.';
				}
				fwrite(sresp, 1, retv, stdout);
				len -= retv;
			}
			if (retv <= 0)
				break;

			// dummy
		} else if (!strcmp(tok, "stat")) {
			printf("Not implemented\n");
			// dummy
		} else if (!strcmp(tok, "quit")) {
			break;
		} else {
			printf("Invalid command.\n");
		}
	}
	close(fd);

	return 0;
}
