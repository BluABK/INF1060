#include <stdio.h>

#include <sys/types.h>
#include <sys/socket.h>

int port = 65530;

int main(int argc, char* argv[]) {
	

	if (argc != 2) {
		printf("Usage: %s [%i]\n", argv[0], port);

	return 0;
}
