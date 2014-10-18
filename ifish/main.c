#include <stdio.h>
#include <errno.h> // stderr

#define DEBUG


int main(void) {

	#ifdef DEBUG
	//fprintf(stderr, "Running in debug mode\n\n");
	printf("Running in debug mode\n\n");
	#endif
	
	printf("I ran!\n");

	return 0;
}
