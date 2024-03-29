#define MAX_LENGTH 120		// Cut off input longer than this amount
#define DELIMITERS " \t\r\n"
#define _GNU_SOURCE		// Define extension for strtok/string.h when forcing -std=c99

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

typedef struct meta_t meta;

struct meta_t {
	meta *next;
	meta *prev;
	int length;
	char index[15];
};

// Set global variables

// General
const char *shell = "ifish";		// Shell name

// History specific
meta *start = NULL;
unsigned long long bitmap = 0;
char histbuf[64*8];

//  Functions
void history_free() {
	meta *cur = start, *prev = NULL;
	if(!cur) return; // If null, nothing to do
	while (cur->next) {
		prev = cur;
		cur = cur->next;
	}
	for (int i = 0; i < cur->length; i++) {
		int index = cur->index[i];
		memset(histbuf + (index * 8), 0, 8);
		bitmap &= ~(1ULL << index);
	}
	free(cur);
	// if prev not null, null prev.next
	if (prev) prev->next = NULL;
	// prev was null (first node deleted)
	else start = NULL;
}


int history_amount_free() {
	int cnt = 0;
	for (int i = 0; i < 64; i++) {
		if ((bitmap &(1 << i)) == 0) {
			cnt++;
		}
	}
	return cnt;
}

int min (int a, int b) {
	if (a < b) return a;
	return b;
}

void history_save(char *cmd) {
	meta *new = malloc(sizeof(meta));
	int len = strlen(cmd);
	if (len % 8) {
		len += 8 - (len % 8);
	}
	len = len/8;
	if (len > MAX_LENGTH) len = MAX_LENGTH;

	while (history_amount_free() < len) {
		history_free();
	}

	for (int i = 0; i < 64 && *cmd && len > 0; i++) {
		if ( bitmap & (1ULL << i) ) continue;

		// copy from memory area 'cmd' to memory area 'histbuf'
		// If string is short enough, add \0, if not cut it off on 8
		if (strlen(cmd) < 8) memcpy(histbuf + (i * 8), cmd, strlen(cmd) +1);
		else memcpy(histbuf + (i * 8), cmd, 8);
		
		// Offset cmd by own length or maximum 8
		cmd += min(8, strlen(cmd));
		bitmap |= (1ULL << i);
		new->index[new->length++] = i;
		len--;
#ifdef DEBUG
		fprintf(stderr, "new->index[%i] = %i\n", new->length-1, i);
#endif
	}
	new->next = start;
	start = new;
}

meta *history_get(int n) {
	meta *cur = start;
	int pos = 0;
	while (cur && pos < n) {
		cur = cur->next;
		pos++;
	}
	return cur;
}

char *history_meta_str(meta *m) {
	if (m == NULL) return NULL;
	static char buf[MAX_LENGTH+1];
	buf[m->length * 8] = 0;
	for (int i = 0; i < m->length; i++) {
		int idx = m->index[i];
		memcpy(buf + (i * 8), histbuf + (idx * 8), 8);
	}
	return buf;
}

int history_cnt() {
	meta *cur = start;
	int cnt = 0;
	while(cur){
		cur = cur->next;
		cnt++;
	}
	return cnt;
}

void history_execute(int n) {

}

void history_delete(int n) {

}

void print_history() {
	int n = 0;
	char *test = history_meta_str(history_get(n));
	if (!test) fprintf(stderr, "history_meta_str(n) returned NULL!\n\n **ABORTED**\n");
	else {
		// Loop over backwards to get last command first
		for (int i = (history_cnt() - 1); i > 0; i--) {
			printf("%i %s\n", i, history_meta_str(history_get(i)));
		}
	}
} 

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
void print_error(const char *sh, char *cmd, int errtype) {
	switch(errtype) {
		case 0:
			#ifdef DEBUG
			fprintf(stderr, "Error: 'errtype' was 0\n");
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

// Debug feedback handlers
#ifdef DEBUG
void print_debug_readline(char *line) {
	fprintf(stderr, "%s (DEBUG) - Read line: ", shell);
	fprintf(stderr, "%s ", line);
	fprintf(stderr, "\n");
}

void print_debug_param(char **param) {
	fprintf(stderr, "%s (DEBUG) - : ", shell);
	for (int i = 0; param[i]; i++) {
		fprintf(stderr, "param[%i] = '%s', ", i, param[i]);
	}
	fprintf(stderr, "\n");
}
#endif

int runc(char *line) {
	char *param[21];
	char *token;
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
	print_debug_readline(line);
#endif
	history_save(line);
#ifdef DEBUG
	fprintf(stderr, "Background? %s\n", background ? "yes" : "no");
#endif

	// Get the first token (command)
	token = strtok(line, DELIMITERS);

	// Iterate throgh the rest (parameters)
	for (i = 0; token != NULL && i < 20;i++) {
		param[i] = token;
		token = strtok(NULL, DELIMITERS);
	}
	param[i] = 0;
#ifdef DEBUG
	print_debug_param(param);
#endif
	// empty lines = ignore
	if(!param[0]) return 1;

#ifdef DEBUG	
	fprintf(stderr, "Saving '");
	for (int i = 0; param[i] != '\0'; i++) {
		fprintf(stderr, "%s ", param[i]);
	}
	fprintf(stderr, "' to history\n");
#endif
	if (strcmp(param[0], "exit") == 0 || strcmp(param[0], "quit") == 0) {
		return 0;
	} else if (strcmp(param[0], "history") == 0 || strcmp(param[0], "h") == 0) {
		if (param[1] && strcmp(param[1], "-d"))
			history_delete(atoi(param[2]));
		else if (param[1])
			history_execute(atoi(param[1]));
		else
			print_history();
	} else {
		pid_t pid = safefork();
		if(pid < 0){
			// error: Seems we have hit our process limit
			fprintf(stderr, "%s: safefork() = -1\n", shell);
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
			if(!background){
				//We are parent, int status; wait(&status); until child is done.. if background, just skip this
				waitpid(pid, &status, 0);
#ifdef DEBUG
				fprintf(stderr, "Child %i exited with code %i\n", WEXITSTATUS(status), pid);
#endif
			} else {
				fprintf(stderr, "Child forked with pid %i\n", pid);
			}
		}
	}
	return 1;
}

int main(void) {
	#ifdef DEBUG
		printf("Running in debug mode\n");
	#endif

	char line[MAX_LENGTH];
	signal(SIGCHLD, SIG_IGN);
	// Program main loop
	while (!feof(stdin)) {
		prompt();
		if (!fgets(line, MAX_LENGTH, stdin)) break;
		if(!runc(line)) break;
	}
	// TODO: Implement history cleanup
	// history_free(); // Until history empty (num_free() == 64)
	return 0;
}
