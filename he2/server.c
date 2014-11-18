#include <stdio.h>
#include <stdlib.h>
#include <string.h> 	// memset
#include <unistd.h> 	// close

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>		// htonl, htons, ntohl, ntohs ?
#include <arpa/inet.h>		// htonl, htons, ntohl, ntohs

int main(int argc, char* argv[]) {
	
	if (argc < 2) {
		printf("Usage: %s [port]\n", argv[0]);
		return 0;
	}

	int port;
	if ((port = atoi(argv[1])) == 0) {
		printf("Invalid port!\n");
		return 0;
	}
	
	int fd = socket(AF_INET, SOCK_STREAM, 0);

	if (fd == -1) {
		perror("socket");
		return -1;
	}

	struct sockaddr_in binfo;
	memset(&binfo, 0, sizeof(binfo));

	binfo.sin_family = AF_INET;
	binfo.sin_port = htons(port);		// 16 bit host to network short

	binfo.sin_addr.s_addr = INADDR_ANY;	// Listen on any IP
	
	// Bind socket
	int retv = bind(fd, (struct sockaddr*)&binfo, sizeof(binfo));

	if(retv != 0) {
		perror("bind");
		return -2;
	}

	retv = listen(fd, 5);
	printf("Listening on port %i\n", port);
	if (retv != 0) {
		perror("listen");
		return -3;
	}

	struct sockaddr_in cinfo;
	memset(&cinfo, 0, sizeof(cinfo));
	socklen_t info_len = sizeof(cinfo);
	int client_fd = accept(fd, (struct sockaddr*)&cinfo, &info_len);
	if (client_fd == -1) {
		perror("accept");
		return -4;
	}
	char cbuf[16]; // TODO: Possibly too ambitious size
	const char *tha_client = inet_ntop(AF_INET, &cinfo.sin_addr.s_addr, cbuf, info_len);
/*	if (retv !=1) {
		perror("inet_ntop");
		return -5;
	}*/

	printf("Client '%s' connected on remote port: %d\n", tha_client, ntohs(cinfo.sin_port));
	
	while (1) {
		char buf[100];
		ssize_t rd = recv(client_fd, buf, sizeof(buf)-1, 0);
		
		if (rd > 0) {
			buf[rd] = 0;
			printf("RECV: %zd bytes from client on fd %d: %s\n", rd, client_fd, buf);
			
			if (buf == "DISCONNECT\n") {
				printf("Client '%s:%d' disconnected from server\n", tha_client, ntohs(cinfo.sin_port));
			}
		} else {
			break;
		}
		
		char *testword = "Heya\n";
		ssize_t sent = send(client_fd, testword, strlen(testword), 0);
		if (sent != -1) {
			printf("SEND: %zd bytes to client on fd %d: %s\n", sent, client_fd, testword);
		} else {
			printf("SENDFAIL: to client on fd %d: %s\n", sent, client_fd, testword);
		}

	}

	close(client_fd);
	close(fd);


	return 0;
}
