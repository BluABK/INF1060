#include <stdio.h>
#include <stdlib.h>
#include <string.h>		// memset
#include <unistd.h>		// close
#include <netdb.h>		// gai_strerror

#include <sys/types.h>		// getaddrinfo, freeaddrinfo, gai_strerror
#include <sys/socket.h>		// getaddrinfo, freeaddrinfo, gai_strerror
#include <netinet/in.h>
#include <arpa/inet.h>		// htonl, htons, ntohl, ntohs
#include <netdb.h>		// getaddrinfo, freeaddrinfo, gai_strerror
/*
typedef struct server_t {
	char rbuf[1024];
	size_t pos;

//	int state;
	char *feed;
	//	char *arg;
} server;
*/
void help() {
	fprintf(stderr, "[1] ls   - list current directory\n");
	fprintf(stderr, "[2] pwd  - get working directory\n");
	fprintf(stderr, "[3] cd   - change working directory\n");
	fprintf(stderr, "[4] get  - get and write file to standard output\n");
	fprintf(stderr, "[5] stat - get file information\n");
	fprintf(stderr, "[?] help\n");
	fprintf(stderr, "[q] quit\n");
}

char *rpath = NULL;

void prompt() {
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
//	server *s; //= &server;
//     	struct server *s = &server;
//	s->pos = 0;
	pos = 0;
//	s->state = 0;
//	s->feed = c->arg = NULL;
	//	ssize_t sent;
	//	ssize_t rd;
	//	char rbuf[100];
	char rbuf;
	//	char * cmd;
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
		connect(fd, cur->ai_addr, cur->ai_addrlen);

		struct sockaddr_in *cur_addr = (struct sockaddr_in*)cur->ai_addr;
		char buf[256];
		buf[0] = 0;

		inet_ntop(AF_INET, &(cur_addr->sin_addr.s_addr), buf, sizeof(buf));

		printf("Connecting to remote host: %s:%s\n", buf, port);

		cur = cur->ai_next;
	}

	// main loop
	help();
	//	while(1) {
	while(prompt(), fgets((void *)input, sizeof(input), stdin)) {

		char *tok = strtok(input, "\n");
		char *arg = strtok(NULL, "\n");

		if (arg != NULL) {
			while (*arg && *arg == ' ') {
				arg++;
			}
		}

		if(!tok) {
			continue;
		} else if (!strcmp(tok, "help")) {
			help();
		} else if (!strcmp(tok, "ls")) {
			send(fd, "ls\n", 3, 0);
			/*
			   rd = recv(fd, rbuf, sizeof(rbuf)-1, 0);
			   if (rd > 0) {
			   rbuf[rd] = 0;
			   printf("DEBUG: RECV: %zd bytes from server on fd %d: %s\n", rd, fd, rbuf);
			   printf("Current directory contains: %s\n", rbuf);
			   }
			*/
		} else if (!strcmp(tok, "pwd")) {
			// Send command to server
			send(fd, "pwd\n", 4, 0);
			
			// Read response char-by-char, and end packet by char '\n'
			int response = 0;
			while (response == 0) {
				retv = read(fd, &rbuf, 1);
				if (retv > 0) {
					if (rbuf == '\n') {
						s->rbuf[s->pos] = 0;
						s->pos = 0;
						if(s->feed != NULL) free(s->feed);
						s->feed = strdup(s->rbuf);
						printf("From socket %d: \\n --> strbuf[%lu] --> rcmd\n", fd, s->pos);
						printf("From socket %d: Parsed string: %s\n", fd, s->feed);
						printf("Current directory: %s\n", s->feed);
						response = 1;
					} else {
						printf("From socket %d: %c --> strbuf[%lu]\n", fd, rbuf, s->pos);
						if(s->pos >= sizeof(s->rbuf)-1) {
							printf("Ignoring because of full buffer\n");
						} else {
							s->rbuf[s->pos++] = rbuf;
						}
					}
				} // if retv > 0
			} // while response
		} else if (!strcmp(tok, "cd")) {
			// dummy
		} else if (!strcmp(tok, "get")) {
			// dummy
		} else if (!strcmp(tok, "stat")) {
			// dummy
		} else if (!strcmp(tok, "quit")) {
			printf("Quitting..\n");
			break;
		} else {
			printf("Invalid command.\n");
		}

		/*
		   const char *testword = "Ohayou~";
		   for (int test = 0; test < 10; test++) {
		   sent = send(fd, testword, strlen(testword), 0);
		// if (send(fd, "Ohayou~\n", 5, 0) < 5) {
		printf("SEND: %zd bytes to server: %s\n", sent, testword);
		if (sent < strlen(testword)) {
		break;
		}
		}
		break;
		}
		*/
		/*
		   sent = send(fd, "pwd\n", 4, 0);
		   rd = recv(fd, rbuf, sizeof(rbuf)-1, 0);

		   if (rd > 0) {
		   rbuf[rd] = 0;
		   printf("RECV: %zd bytes from server on fd %d: %s\n", rd, fd, rbuf);
		   }
		   */
		send(fd, "stat\nloremipsum.txt\n", 20, 0);
		/*
		   rd = recv(fd, rbuf, sizeof(rbuf)-1, 0);

		   if (rd > 0) {
		   rbuf[rd] = 0;
		   printf("RECV: %zd bytes from server on fd %d: %s\n", rd, fd, rbuf);
		   }
		   */
		break;
	}
	printf("Closing filedescriptor\n");
	close(fd);
	printf("Freeing up addriinfo(res)\n");

	freeaddrinfo(res);
	return 0;
}
