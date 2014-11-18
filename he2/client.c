#include <stdio.h>
#include <stdlib.h>
#include <string.h>	// memset
#include <unistd.h>	// close

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>		// htonl, htons, ntohl, ntohs
#include <netdb.h>

int main(int argc, char* argv[]) {
	
	int retv = 0;

	if (argc < 3) {
		printf("Usage: %s [host] [port]\n", argv[0]);
		return 0;
	}
	
	// port
/*	int port;
	
	if ((port = atoi(argv[2])) == 0) {
		printf("Invalid port!\n");
		return 0;
	}
	*/
	char *port = argv[2];
/*
	struct in_addr addr;
	memset(&addr, 0, sizeof(addr));

	retv = inet_pton(AF_INET, argv[1], &addr);
	if (retv != 1) {
		printf("inet_pton failed!\n");
		return -1;
	}
*/
	// getaddrinfo
	const char *hostname = argv[1];
	// move port here if working
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
/*	int port;
	if ((port = atoi(argv[2])) == 0) {
		printf("Invalid port!\n");
		return 0;
	}

	struct in_addr addr;
	memset(&addr, 0, sizeof(addr));

	retv = inet_pton(AF_INET, argv[1], &addr);
	if (retv != 1) {
		printf("inet_pton failed!\n");
		return -1;
	}
*/

//	int fd = socket(AF_INET, SOCK_STREAM, 0);
/*	struct sockaddr_in other;
	memset(&other, 0, sizeof(other));
	
	other.sin_family = AF_INET;
	other.sin_port = htons(port);
	other.sin_addr = addr;
*/
/*	retv = connect(fd, (struct sockaddr*)&other, sizeof(other));
	if (retv != 0) {
		perror("connect");
		return -2;
	}
*/	
	while(1) {
		ssize_t sent = send(fd, "Ohayou~\n", 8, 0);
//		if (send(fd, "Ohayou~\n", 5, 0) < 5) {
		printf("Sent %zd bytes\n", sent);
		if (sent < 8) {
			break;
		}
	}

	close(fd);
	freeaddrinfo(res);
	return 0;
}
