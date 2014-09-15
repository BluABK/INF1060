#include <stdio.h>
#include <string.h>

void print_file(char *filedata) {
	printf("File contents:\n\n");
	int i;
	for (i = 0; filedata[i] != '\0'; i++) {
		printf("%c", filedata[i]);
	}
	printf("\n");
}

void encode_file() {
	// Dummy function
}

void decode_file() {
	// Dummy function
}

void help() {
        const char *helpmsg =
                "USAGE: ./oppg2 command input_file output_file\n"
                "\n"
                "where \"command\" is one of the following:\n"
                "\n"
                "\tp      print input_file (output_file is ignored if specified)\n"
                "\te      encode/compress input_file to output_file\n"
                "\td      decode/uncompress and print input_file\n"
                "\thelp   display this help message\n";

        printf(helpmsg);
}

char *read_file(char *filename) {
	FILE *file = fopen(filename, "r");
	char *content;
//	size_t n = 0;
//	int c;

	if (file == NULL) {
		printf("Failed to open file '%s', did you make a typo?\n", filename);
		return NULL;
	}

	// Determine file size
	fseek(file, 0, SEEK_END);
	long f_size = ftell(file);
	fseek(file, 0, SEEK_SET);
	content = malloc(f_size);

//	while ((c = fgetc(file) != EOF)) {
//		content[n++] = (char)c;
//	}

	// Read file into data variable
	fread(content, f_size, 1, file);

	// Set end of string
//	data[n] = '\0';

	// Close the file handle
	fclose(file);

	// return the data variable
	return content;
}

int main(int argc, char *argv[]) {
	char *data;

	// If an input file was specified
	if (argc == 3) {
		data = malloc(strlen(read_file(argv[2])));
		strcpy(read_file(argv[2]), data);
		printf("DEBUG:\tRead file\n");
		printf("%s", data);
	}

	// Check for arguments and run the appropriate function
	if 	(!strcmp(argv[1], "p") && argc > 2)		print_file(data);
	else if (!strcmp(argv[1], "e") && argc > 3)		encode_file();
	else if (!strcmp(argv[1], "d") && argc > 3)		decode_file();
	else							help();

	return 0;
}
