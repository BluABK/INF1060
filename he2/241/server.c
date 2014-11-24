#include <stdio.h>
#include <stdlib.h>
#include <string.h> 		// memset
#include <unistd.h> 		// close
#include <dirent.h>
#include <netinet/in.h>		// htonl, htons, ntohl, ntohs
#include <sys/select.h>		// fd_set
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>

typedef struct client_t {
	char buf[1024];
	size_t pos;

	int state;
	char *cmd;
	char *arg;
	char cwd[256];
} client;

void run_ls(int fd, client *c) {
	DIR *d = opendir(c->cwd);
	struct dirent *e;
	if(!d) {
		perror("opendir");
		write(fd, "\n", 1);
		return;
	}
	while((e = readdir(d)) != NULL) {
		write(fd, e->d_name, strlen(e->d_name));
		write(fd, "\n", 1);
	}
	write(fd, "\n", 1);
	closedir(d);
}
void run_get(int fd, client *c) {
	char absolute[strlen(c->cwd)+strlen(c->arg)+2];
	struct stat a;
	FILE *f;

	strcpy(absolute, c->cwd);
	strcat(absolute, "/");
	strcat(absolute, c->arg);

	if(lstat(absolute, &a) < 0) {
		write(fd, "error\n", 6);
		return;
	}
	if(!S_ISREG(a.st_mode)) {
		write(fd, "error\n", 6);
		return;
	}
	f = fopen(absolute, "rb");
	if(!f) {
		write(fd, "error\n", 6);
		return;
	}
	char num[64];
	snprintf(num, sizeof(num), "%lu\n", a.st_size);
	write(fd, num, strlen(num));
	
	unsigned int remaining = a.st_size;
	char buf[4096];
	while(remaining) {
		size_t toread = sizeof(buf);
		if(remaining < toread) toread = remaining;
		int retv = fread(buf, 1, toread, f);
		if(retv < 0) {
			perror("fread");
			printf("Reading file stopped midway, client is now stuck\n");
			break;
		}
		write(fd, buf, retv);
		remaining -= retv;
	}
	fclose(f);
}

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
	int request_sd;
	int strpos = 0;
	char buf;
	client *c, clients[FD_SETSIZE];
	fd_set fds, readfds;

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
	FD_SET(request_sd, &fds);

	// Main loop
	for (;;) {
		readfds = fds;
		retv = select(FD_SETSIZE, &readfds, NULL, NULL, NULL);

		// Something went wrong
		if (retv == -1) {
			perror("select()");
			printf("select() returned %i\n", retv);
			return -4;
		}
		// nothing happened; select continued
		if (retv == 0) {
			printf("Connection timeout\n");

			for (i = 0; i < FD_SETSIZE; i++) {
				if(!FD_ISSET(1, &fds)) continue;
				// Send a response
				write(i, "Server ACK", 11);
				// Close sockets
				close(i);
				FD_CLR(i, &fds);
			}
			return 0;
		}
		for (i = 0; i < FD_SETSIZE; i++) {
			if (FD_ISSET(i, &readfds)) {
				if (i == request_sd) {
					// New connection request
					printf("Client connected on sd %i\n", request_sd);
					clientaddrlen = sizeof(clientaddr);
					retv = accept(request_sd, (struct sockaddr *)&clientaddr, (socklen_t *)&clientaddrlen);
					if (retv >= 0) {
						c = &clients[retv];
						c->pos = 0;
						c->state = 0;
						c->cmd = c->arg = NULL;
						getcwd(c->cwd, sizeof(c->cwd));
						FD_SET(retv, &fds);
					} else if(retv >= FD_SETSIZE) {
						printf("Too high fd number for select\n");
						close(retv);
					} else {
						perror("accept()");
						return -5;
					}
				} else {
					// Data arrived on an existing socket - clientstuff
					c = &clients[i];
					// Read char-by-char, separate command, ifreq param and command packet end by the byte char \n
					retv = read(i, &buf, 1);

					if (retv > 0) {
						if (buf == '\n') {
							c->buf[c->pos] = 0;
							c->pos = 0;
							if(!c->state) {
								// !state: recv cmd
								if(c->cmd != NULL) free(c->cmd);
								c->cmd = strdup(c->buf);
								printf("From socket %d: bufstat unset: the following is a command:\n", i);
								printf("From socket %d: Parsed string: %s\n", i, c->cmd);

								if (strcmp(c->cmd, "pwd") == 0) {
									printf("From socket %d: Client requested cmd: pwd\n", i);
									//run_cmd(i, "pwd", NULL);
									
									send(i, c->cwd, sizeof(c->cwd), 0);
									send(i, "\n", 1, 0);
								} else if (strcmp(c->cmd, "ls") == 0) {
									printf("From socket %d: Client requested cmd: ls\n", i);
									run_ls(i, c);
								} else if (strcmp(c->cmd, "cd") == 0) {
									printf("From socket %d: Client requested cmd: cd\n", i);
									c->state = 1;
								} else if (strcmp(c->cmd, "get") == 0) {
									printf("From socket %d: Client requested cmd: get\n", i);
									c->state = 1;
								} else if (strcmp(c->cmd, "stat") == 0) {
									printf("From socket %d: Client requested cmd: stat\n", i);
									c->state = 1;
								} else {
									printf("From socket %d: Recieved cmd/newline, but no match occured\n", i);
								}
							} else { // state != 0: recv arg
								if(c->arg != NULL) free(c->arg);
								c->arg = strdup(c->buf);

								printf("From socket %d: bufstat set: the following is an argument:\n", i);
								printf("From socket %d: \\n --> strbuf[%d] --> rarg\n", i, strpos);
								printf("From socket %d: Parsed string: %s\n", i, c->arg);
								
								printf("From socket %d: Client requested cmd w/ arg: %s %s\n", i, c->cmd, c->arg);
								if (!strcmp(c->cmd, "get")) {
									run_get(i, c);
								}
								c->state = 0;
							}
						} else {
							if(c->pos >= sizeof(c->buf)-1) {
								printf("Ignoring because of full buffer\n");
							} else {
								c->buf[c->pos++] = buf;
							}
						}
					} else if (retv <= 0) {
						close(i);
						if(c->cmd) free(c->cmd);
						if(c->arg) free(c->arg);
						FD_CLR(i, &fds);

					}
				} // else 
			} // if (FDISSET
		} // for FD_SETSIZE
		// code moved to end of file
	} // for(;;)

	close(request_sd);
	//	close(client_fd);
	//	close(fd);
	return 0;
}
