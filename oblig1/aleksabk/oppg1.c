#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

// Create the linked list node
struct node {
	char *ptr;
	struct node *next;
};

// Linked list to store file data
struct list {
	struct node *root;
};

// Function declarations

void print_file(struct node *list) {
	// Traverse linked list and print line-by-line
	printf("File contents:\n");
	// Create a temporary node ptr to hold the list
	struct node *tmp = list;

	// Traverse the list
	while(tmp) {
		printf("\t%s", tmp->ptr);
		tmp = tmp->next;
	}
	printf("\n");
}

void print_random_ln(struct node *list, int list_length) {
	// Pick and print a random line from the list
	printf("Random line:\n");
	// Create a temporary node ptr to hold the list
	struct node *tmp = list;
	// Initialize rand with a seed
	srand(time(NULL));
	// Generate a random number between 0 and list length
	int rnd = rand() % list_length;
	int count = 0;

	// Traverse the list
	while(tmp) {
		if (count == rnd) {
			printf("\t%s\n", tmp->ptr);
			// We're done here, end the loop
			break;
		}
		tmp = tmp->next;
		count++;
	}
}
// TODO: Finish
void replace_vow(struct node *list, char *vowels_lower, char *vowels_upper) {
	// Traverse the list while replacing all vowels and then print the result
	printf("Replace vowels:\n");
	// Create a temporary node ptr to hold the list
	struct node *tmp = list;
	
	// Traverse the list
	while(tmp) {
		
		tmp = tmp->next;		
	}
}

void remove_vow(struct node *list, char *vowels_lower, char *vowels_upper) {
	// Traverse the list while removing all vowels and then print the result
	printf("Remove vowels:\n");
	// Create a temporary node ptr to hold the list
	struct node *tmp = list;
	
	// Traverse the list
	while(tmp) {
		printf("\t");
		// Traverse the line item in the list
		int i;
		for (i = 0; tmp->ptr[i] != '\0'; i++) {
			if (strchr(vowels_lower, tmp->ptr[i]) || strchr(vowels_upper, tmp->ptr[i])) continue;
			printf("%c", tmp->ptr[i]);
		}
		// Jump to next item in the list
		tmp = tmp->next;
	}
	printf("\n");
}

void print_len(struct node *list) {
	// Create a temporary node ptr to hold the list
	struct node *tmp = list;
	// Store the string length here
	int file_length = 0;
	while(tmp) {
		file_length = file_length + strlen(tmp->ptr);
		tmp = tmp->next;
	}
	printf("The text is %d characters long\n", file_length);
}

void help() {
	const char *helpmsg =
		"USAGE: ./oppgi command input_file\n"
		"\n"
		"where \"command\" is one of the following:\n"
		"\n"
		"\tprint        print input_file\n"
		"\trandom       print a random line\n"
		"\treplace      replace the vowels with all the other vowels\n"
		"\tremove       remove vowels\n"
		"\tlen          print the number of characters in the input_file\n"
		"\thelp         display this help message\n";

	printf(helpmsg);
}

int main(int argc, char *argv[]) {
	struct node *list_start = NULL;
	struct node *list_end   = NULL;
	struct node *tmpnode	= NULL;
	int list_count          = 0;
	char vowels_lower[]	= "aeiouy\xe6\xf8\xe5";
	char vowels_upper[]	= "AEIOUY\xc6\xd8\xc5";
	char line[1024];
	FILE *fp;
	if (argc == 3) {
		// Create linked list
		fp = fopen(argv[2], "r");
		if (fp) {
			while (fgets(line, sizeof(line), fp)) {
				// Set a maximum of 1023 characters, removes the last character (newline)
				//line[strlen(line)-1] = 0;
				
				tmpnode = (struct node *) malloc(sizeof(struct node));
				tmpnode->ptr = malloc(strlen(line)+1); 
				strcpy(tmpnode->ptr, line);
				
				// Insert line into linked list
				if(!list_end){
					// this is the first time we insert
					list_start = tmpnode;
					list_end = tmpnode;
				} else {
					// This is just another node
					list_end->next = tmpnode;
					list_end = tmpnode;
				}
				list_count++;
			}
			// If the file handle is open, close it.
			fclose(fp);
		} else {
			printf("Failed to open file '%s', did you make a typo?\n", argv[2]);
			return 1;
		}

		// Check for arguments and run appropriate function
		if (!strcmp(argv[1], "print"))		print_file(list_start);
		else if (!strcmp(argv[1], "random"))	print_random_ln(list_start, list_count);
		else if (!strcmp(argv[1], "replace"))	replace_vow(list_start, vowels_lower, vowels_upper);
		else if (!strcmp(argv[1], "remove"))	remove_vow(list_start, vowels_lower, vowels_upper);
		else if (!strcmp(argv[1], "len"))	print_len(list_start);
		else 					help();

		// destroy list
		// REMOVEME: comments
		// as long as we have a list:
		// 	tmpnode is the start of the list's next
		// 	free the head of the list
		// 	set the new head of the list to tmpnode (when next->NULL then the new head will be set to NULL automagically)
		while (list_start) {
			tmpnode = list_start->next;
			free(list_start->ptr);
			free(list_start);
			list_count--;
			list_start = tmpnode;
		}
		list_end = NULL;
	} else {
                if (argc < 3) printf("You have entered too few arguments.\n");
		if (argc > 3) printf("You have entered too many arguments.\n");
		
                help();
	}

	return 0;
}
