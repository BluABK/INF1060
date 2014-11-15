#include <stdio.h>
#include <stdlib.h>
#include <string.h>	// memset
#include <unistd.h>	// close

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>		// htonl, htons, ntohl, ntohs

int main(int argc, char* argv[]) {
	
	if (argc > 3) {
		printf("Usage: %s [host] [port]\n", argv[0]);
		return 0;
	}
	
//	int port = atoi(argv[2]);
	int port;
	if ((port = atoi(argv[2])) == 0) {
		printf("Invalid port!\n");
		return 0;
	}

	struct in_addr addr;
	memset(&addr, 0, sizeof(addr));

	int retv = inet_pton(AF_INET, argv[1], &addr);
	if (retv != 1) {
		printf("inet_pton failed!\n");
		return -1;
	}

	int fd = socket(AF_INET, SOCK_STREAM, 0);
	struct sockaddr_in other;
	memset(&other, 0, sizeof(other));
	
	other.sin_family = AF_INET;
	other.sin_port = htons(port);
	other.sin_addr = addr;

	retv = connect(fd, (struct sockaddr*)&other, sizeof(other));
	if (retv != 0) {
		perror("connect");
		return -2;
	}
	
	while(1) {
		ssize_t sent = send(fd, "Ohayou~\n", 8, 0);
//		if (send(fd, "Ohayou~\n", 5, 0) < 5) {
		printf("Sent %zd bytes\n", sent);
		if (sent < 8) {
			break;
		}
	}

	close(fd);

	return 0;
}
