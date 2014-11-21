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

int main(int argc, char* argv[]) {
	
	int retv = 0;
	if (argc < 3) {
		printf("Usage: %s [host] [port]\n", argv[0]);
		return 0;
	}
	
	// port
	// TODO: Find an alternative way to validate port input
	
	// getaddrinfo
	const char *hostname = argv[1];
	char *port = argv[2];
	struct addrinfo hints;
	struct addrinfo *res;

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
		char buf[100];
		buf[0] = 0;

		inet_ntop(AF_INET, &(cur_addr->sin_addr.s_addr), buf, sizeof(buf));
		
		printf("Connecting to remote host: %s:%s\n", buf, port);

		cur = cur->ai_next;
	}
	
	ssize_t sent;
	char * cmd;
	while(1) {
		const char *testword = "Ohayou~\n";
		for (int test = 0; test < 10; test++) {
			sent = send(fd, testword, strlen(testword), 0);
			// if (send(fd, "Ohayou~\n", 5, 0) < 5) {
			printf("SEND: %zd bytes to server: %s", sent, testword);
			if (sent < strlen(testword)) {
				break;
			}
		}
		break;
	}
	cmd = "DISCONNECT\n";
	sent = send(fd, cmd, strlen(cmd), 0);
	printf("SEND: %zd bytes: %s", sent, cmd);
	printf("Closing filedescriptor\n");
	close(fd);
	printf("Freeing up addriinfo(res)\n");
	freeaddrinfo(res);
	return 0;
}
