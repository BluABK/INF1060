#include <stdio.h>
#include <stdlib.h>	// getenv & friends
#include <errno.h>	// stderr
#include <stdbool.h>	// boolean supporti
#include <unistd.h>	// UNIX

// Define some constants
#define MAX_LENGTH 120

#define DEBUG

// Set global variables
char *shell = "ifish";
bool run = true;
int cnt = 0;

void prompt(int cmd_cnt) {
	//return getenv("USER") + "@" + "ifish " + cmd_cnt + ":" + getenv("PATH") +"$ ";
	printf( "%s@%s %d:%s> ", getenv("USER"), shell, cmd_cnt, getenv("PWD") );
}

// Program exit handler
void quit() {
	printf("\n");
	run = false;
}

int main(int argc, char *argv[]) {
	#ifdef DEBUG
		printf("Running in debug mode\n\n");
	#endif
	char line[MAX_LENGTH];

	// Program main loop
	while (run) {
	//	prompt = set_prompt(cnt++);
	//	printf(prompt);
		prompt(cnt++);
		if (!fgets(line, MAX_LENGTH, stdin)) run = false;
		system(line);
	}	

	return 0;
}
