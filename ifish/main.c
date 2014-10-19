#define MAX_LENGTH 120		// Cut off input longer than this amount
#define DELIMITERS " \t\r\n"
#define _GNU_SOURCE		// Define extension for strtok/string.h when forcing -std=c99
//#define DEBUG			// Ahh, verbosity..

#include <stdio.h>
#include <stdlib.h>		// getenv, free & friends
#include <errno.h>		// stderr
#include <stdbool.h>		// boolean support
#include <unistd.h>		// UNIX Standard library
#include <string.h>		// strtok

// Linked list node

struct node {
	char *ptr;
	struct node *next;
	struct node *prev;
};

struct cmd_history {
	struct node *root;
};


// Set global variables
char *shell = "ifish";		// Shell name
bool run = true;		// Main loop
int cnt = 0;			// Command counter

//  Functions

void prompt(int cmd_cnt) {
	printf( "%s@%s %d:%s> ", getenv("USER"), shell, cmd_cnt, getenv("PWD") );
}

// Error feedback handler
void print_error(char *sh, char *cmd, int errtype) {
	switch(errtype) {
		case 0:
			#ifdef DEBUG
			fprintf(stderr, "Hm.. There's a mystery afoot. This simply should not happen!\n");
			#endif
			break;
		case 1:	
			printf("%s: %s: command not found\n", sh, cmd);
			break;
		case 2:
			printf("%s: %s: command not implemented\n", sh, cmd);
			break;
	}
}

void print_debug(char *sh, char *cmd) {
	fprintf(stderr, "%s (DEBUG) - Read line: %s\n", shell, cmd);
}

// Program exit handler
void quit() {
	printf("\n");
	run = false;
}

int main(int argc, char *argv[]) {
	#ifdef DEBUG
		printf("Running in debug mode\n");
	#endif

	char line[MAX_LENGTH];
	char *cmd;

	// Program main loop
	while (run) {
		prompt(cnt++);
		if (!fgets(line, MAX_LENGTH, stdin)) run = false;

		// Parse and execute command
		if ((cmd = strtok(line, DELIMITERS))) {
		#ifdef DEBUG
			print_debug(shell, cmd);
		#endif
			// Reset errors
			errno = 0;

			if (strcmp(cmd, "exit") == 0 || strcmp(cmd, "quit") == 0) {
				quit();
			} else if (strcmp(cmd, "history") == 0 || strcmp(cmd, "h") == 0) {
				print_error(shell, cmd, 2);
			} else if (strcmp(cmd, "derp") == 0) {
				print_error(shell, cmd, 0);
			} else system(line);
			if (errno) print_error(shell, cmd, 1);
		}
	}	

	return 0;
}
