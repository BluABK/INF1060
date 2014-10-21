#define MAX_LENGTH 120		// Cut off input longer than this amount
#define DELIMITERS " \t\r\n"
#define _GNU_SOURCE		// Define extension for strtok/string.h when forcing -std=c99
//#define DEBUG			// Ahh, verbosity..

#include <stdio.h>
#include <stdlib.h>		// getenv, free & friends
#include <errno.h>		// stderr
#include <stdbool.h>		// boolean support
#include <unistd.h>		// UNIX Standard library (__environ)
#include <string.h>		// strtok
#include <ctype.h>		// isspace
#include <sys/wait.h>		// waitpid / wait

#include "safefork.h"

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

//  Functions

void prompt() {
	static int cnt = 1;
	printf( "%s@%s %d:%s> ", getenv("USER"), shell, cnt++, getenv("PWD") );
}

char *find_path(char *cmd) {
	static char result[256];
	char * path = strdup(getenv("PATH"));
	char *token;

	token = strtok(path, ":");

	while (token) {
		result[0] = 0;
#ifdef DEBUG
		printf("token = %s\n", token);
#endif
		strcat(result, token);
		strcat(result, "/");
		strcat(result, cmd);

		if (access(result, X_OK) == 0) {
			free(path);
			return result;
		}
		token = strtok(NULL, ":");
	}
	free(path);
	return NULL;
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
#ifdef DEBUG
void print_debug(char *sh, char **param) {
	fprintf(stderr, "%s (DEBUG) - Read line: ", shell);
	for (int i = 0; param[i]; i++) {
	fprintf(stderr, "%s", param[i]);
	}
	fprintf(stderr, "\n");
}
#endif

void runc(char *line, bool run) {
	char *param[21];
	char * token;
	char *path;
	bool background = false;
	int i, job;
	
	// Parse and execute command
	background = false;
	for (i = strlen(line)-1; i >= 0; i--) {
		if (isspace(line[i]))
			line[i] = 0;
		else if (line[i] == '&') {
		       	background=true;
			line[i] = 0;
		}
		else break;
	}
#ifdef DEBUG
	fprintf(stderr, "Background? %s\n", background ? "yes" : "no");
#endif

	// Get the first token (command)
	token = strtok(line, DELIMITERS);

	// Iterate throgh the rest (parameters)
	for (i = 0; token != NULL && i < 20;i++) {
#ifdef DEBUG
			printf("%s --> param[%d]\n", token, i);
#endif

		param[i] = token;
		token = strtok(NULL, DELIMITERS);
	}
	param[i] = 0;
#ifdef DEBUG
	print_debug(shell, param);
	fprintf(stderr, "param[0] is at %p\n", param[0]);
#endif
	// empty lines = ignore
	if(!param[0]) return;

	// Reset errors
	errno = 0;

	if (strcmp(param[0], "exit") == 0 || strcmp(param[0], "quit") == 0) {
		run = false;
		return;
	} else if (strcmp(param[0], "history") == 0 || strcmp(param[0], "h") == 0) {
		print_error(shell, param[0], 2);
	} else if (strcmp(param[0], "derp") == 0) {
		print_error(shell, param[0], 0);
	} else {
		// lrn 2 run programs!!! stupid shell
		// TODO: safefork()
		// TODO: child searches path, checks access() and executes, then DIES!!!
		// TODO: parent calls wait(&status) to wait for child to DIEEEE
		// if safefork returns -1: error and continue to top
		// returns > 0: we are parent, call wait() and then return to top
		// returns 0: we are child, do our stuff and exit (DO NOT LET IT AVOID EXIT)
		//
		//
		// TODO in backgrounding:
		// * every time we background a process, we have to throw that process ID into a list
		// * after every line read, we call waitpid with WNOHANG. for each pid that has returned we can remove it from the list
		// * at the end of the program, wait on all childs left (blocking)
		pid_t pid = safefork();
		if(pid < 0){
			// error: Seems we have hit our process limit
		} else if(pid == 0){
			// I'm a child!! yey
			path = find_path(param[0]);
			
			// in case execve fails to execute, we have to exit ourselves
			if (path == NULL) {
				print_error(shell, param[0], 1);
				exit(127);
			} else {
				job = execve(path, param, __environ);
				exit(job); 
			}
		} else {
			int status;
			// advanced edition, waits on specific pid (in case any backgrounded process returns first)
#ifdef DEBUG
			fprintf(stderr, "Child forked with pid %i\n", pid);
#endif
			if(!background){
				waitpid(pid, &status, 0);
#ifdef DEBUG
				fprintf(stderr, "Child %i exited with code %i\n", WEXITSTATUS(status), pid);
#endif
				// we are parent, int status; wait(&status); until child is done.. if background, just skip this (when you feel like getting better grades, read up on waitpid()
			}
		}
	}
	if (errno) print_error(shell, param[0], 1);
}

// Zombie apocalypse deterrent
void zombie_deterrent() {
	pid_t pid;
	int status;
	while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
		fprintf(stderr, "Child %i exited with code %i\n", WEXITSTATUS(status), pid);
	}
}

int main(void) {
	#ifdef DEBUG
		printf("Running in debug mode\n");
	#endif

	char line[MAX_LENGTH];
	bool run = true;

	// Program main loop
	while (!feof(stdin) && run == true) {
		prompt();
		if (!fgets(line, MAX_LENGTH, stdin)) break;
		zombie_deterrent();
		runc(line, run);
	}
	zombie_deterrent();
	return 0;
}
