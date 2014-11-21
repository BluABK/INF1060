#include <stdio.h>
#include <stdlib.h>
#include <string.h> 	// memset
#include <unistd.h> 	// close
#include <time.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>		// htonl, htons, ntohl, ntohs ?
#include <arpa/inet.h>		// htonl, htons, ntohl, ntohs
#include <sys/select.h>		// fd_set

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
	
	// Declarations
	struct sockaddr_in serveraddr;
	struct sockaddr_in clientaddr;
	int clientaddrlen, i, retv;
	int request_sd, sd[2], numsocks, maxsocks;
	char buf[13];
	fd_set fds, readfds, writefds, exceptfds;
	struct timeval timeout;

	numsocks = 0, maxsocks = 2;
	timeout.tv_sec = 20;
	timeout.tv_usec = 0;
	// Create Socket
	request_sd = socket(AF_INET, SOCK_STREAM, 0);
	
	// Fill in the address structure
	memset(&serveraddr, 0, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(port);		// 16 bit host to network short
	serveraddr.sin_addr.s_addr = INADDR_ANY;	// Listen on any IP

	// Bind address to socket
	retv = bind(request_sd, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
	if(retv != 0) {
		perror("bind()");
		return -2;
	}
	// Activate the connect request queue
	retv = listen(request_sd, SOMAXCONN);
	printf("Listening on port %i\n", port);
	if (retv != 0) {
		perror("listen()");
		return -3;
	}

	// Initialise fd set
	FD_ZERO(&fds);
//	FD_SET(request_sd, &fds);	// makes select() return -1 o0
	FD_SET(0, &fds);
	printf("DEBUG: FD_SETSIZE = %i\n", FD_SETSIZE);
	printf("DEBUG: request_sd = %i\n", request_sd);
	// Main loop
	for (;;) {
		readfds = fds;
		retv = select(FD_SETSIZE, &readfds, NULL, NULL, &timeout);
	//	retv = select(1, &readfds, NULL, NULL, &timeout);

		// Something went wrong
		if (retv == -1) {
			perror("select()");
			printf("select() returned %i\n", retv);
			return -4;
		}
		// nothing happened; select continued
		if (retv == 0) {
			printf("Connection timeout\n");
			
			for (i = 0; i < numsocks; i++) {
				// Send a response
				write(sd[i], "Server ACK", 11);
				// Close sockets
				close(sd[i]);
				FD_CLR(sd[i], &fds);
			}
			return 0;
		}
		
		for (i = 0; i < FD_SETSIZE; i++) {
			if (FD_ISSET(i, &readfds)) {
				if (i == request_sd) {
					// New connection request
					if (numsocks < maxsocks) {
						sd[numsocks] = accept(request_sd, (struct sockaddr *)&clientaddr, (socklen_t *)&clientaddrlen);
						FD_SET(sd[numsocks], &fds);
						numsocks++;
					} else {
						printf("Ran out of socket space.\n");
						return -5;
					}
				} else {
					// Data arrived on an existing socket - clientstuff
					read(i, buf, 12);
					buf[12] = 0;
					printf("From socket. %d: %s\n", i, buf);
				}
			}
		}
	close(request_sd);

	/*
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
	if (retv !=1) {
		perror("inet_ntop");
		return -5;
	}*/
/*
	printf("Client '%s' connected on remote port: %d\n", tha_client, ntohs(cinfo.sin_port));
	
	while (1) {
		char buf[100];
		ssize_t rd = recv(client_fd, buf, sizeof(buf)-1, 0);
		
		if (rd > 0) {
			buf[rd] = 0;
			printf("RECV: %zd bytes from client on fd %d: %s\n", rd, client_fd, buf);
			
			if (!strcmp(buf, "DISCONNECT\n")) {
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
			printf("SENDFAIL: %li bytes to client on fd %d: %s\n", sent, client_fd, testword);
		}
		*/
	}

//	close(client_fd);
//	close(fd);
	return 0;
}
