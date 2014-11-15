#include <stdio.h>


int main(int argc, char* argv[]) {
	
	if (argv > 3) {
		printf("Usage: %s [host] [port]\n", argv[0]);
		return 0;
	}
	
//	int port = atoi(argv[2]);
	int port;
	if ((port = atoi(argv[1])) == 0) {
		printf("Invalid port!\n");
		return 0;
	}




	return 0;
}
