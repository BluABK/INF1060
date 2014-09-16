#include <stdio.h>  // Required for I/O functions
#include <string.h> // Required for str* functions
#include <stdlib.h> // Required for free() and rand()
#include <time.h>   // Required for seeding rand()

// Define linked list node
struct node {
	char *ptr;
	struct node *next;
};

// Linked list to store file data
struct list {
	struct node *root;
};

void print_file(struct node *list) {
	// Create a temporary node ptr to hold the list
	struct node *tmp = list;
	// Traverse linked list and print line-by-line
	printf("File contents:\n");

	// Traverse the list
	while(tmp) {
		printf("\t%s", tmp->ptr);
		tmp = tmp->next;
	}
	printf("\n");
}

void print_random_ln(struct node *list, int list_length) {
	// Create a temporary node ptr to hold the list
	struct node *tmp = list;
	int rnd = 0;
	int count = 0;

	// Pick and print a random line from the list
	printf("Random line:\n");
	// Initialize rand with a seed
	srand(time(NULL));
	// Generate a random number between 0 and list length
	rnd = rand() % list_length;

	// Traverse the list
	while(tmp) {
		if(count == rnd) {
			printf("\t%s\n", tmp->ptr);
			// We're done here, end the loop
			break;
		}
		tmp = tmp->next;
		count++;
	}
}
void replace_vow(struct node *list, char *vowels_lower, char *vowels_upper) {
	// Create a temporary node ptr to hold the list
	struct node *tmp = list;
	int i, j;
	// Traverse the list while replacing all vowels and then print the result
	printf("Replace vowels:\n");

	// Traverse the list for every vocal letter
	for(i = 0; vowels_lower[i]; i++) {
		printf("\t... with vowel '%c':\n", vowels_lower[i]);
		// Traverse the list
		tmp = list;
		while(tmp) {
			putchar('\t');
			// Traverse the line item in the list
			for(j = 0; tmp->ptr[j]; j++) {
				if(strchr(vowels_lower, tmp->ptr[j]) || strchr(vowels_upper, tmp->ptr[j])) {
					putchar(vowels_lower[i]);
					continue;
				}
				putchar(tmp->ptr[j]);
			}
			// Jump to the next item in the list
			tmp = tmp->next;
		}
		putchar('\n');
		putchar('\n');
	}
	putchar('\n');
}

void remove_vow(struct node *list, char *vowels_lower, char *vowels_upper) {
	// Create a temporary node ptr to hold the list
	struct node *tmp = list;
	int i;
	// Traverse the list while removing all vowels and then print the result
	printf("Remove vowels:\n");

	// Traverse the list
	while(tmp) {
		putchar('\t');
		// Traverse the line item in the list
		for(i = 0; tmp->ptr[i]; i++) {
			if(strchr(vowels_lower, tmp->ptr[i]) || strchr(vowels_upper, tmp->ptr[i]))
				continue;
			putchar(tmp->ptr[i]);
		}
		// Jump to next item in the list
		tmp = tmp->next;
	}
	putchar('\n');
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
		"USAGE: ./oppg1 command input_file\n"
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
	struct node *tmpnode    = NULL;
	int list_count          = 0;
	char vowels_lower[]     = "aeiouy\xe6\xf8\xe5";
	char vowels_upper[]     = "AEIOUY\xc6\xd8\xc5";
	char line[1024];
	FILE *fp;
	if(argc == 3) {
		// Create linked list
		fp = fopen(argv[2], "r");
		if(fp) {
			while(fgets(line, sizeof(line), fp)) {
				tmpnode      = (struct node *) malloc(sizeof(struct node));
				tmpnode->ptr = malloc(strlen(line)+1);
				strcpy(tmpnode->ptr, line);

				// Insert line into linked list
				if(!list_end) {
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
		if     (!strcmp(argv[1], "print"))   print_file(list_start);
		else if(!strcmp(argv[1], "random"))  print_random_ln(list_start, list_count);
		else if(!strcmp(argv[1], "replace")) replace_vow(list_start, vowels_lower, vowels_upper);
		else if(!strcmp(argv[1], "remove"))  remove_vow(list_start, vowels_lower, vowels_upper);
		else if(!strcmp(argv[1], "len"))     print_len(list_start);
		else                                 help();

		// Destroy list - as long as we have a list:
		while(list_start) {
			// tmpnode is the start of the list's next
			tmpnode = list_start->next;
			// free the head of the list
			free(list_start->ptr);
			free(list_start);
			// set the new head of the list to tmpnode (when next->NULL then the new head will be set to NULL automagically)
			list_count--;
			list_start = tmpnode;
		}
		list_end = NULL;
	} else {
                if(argc < 3) printf("You have entered too few arguments.\n");
		if(argc > 3) printf("You have entered too many arguments.\n");

                help();
	}

	return 0;
}
