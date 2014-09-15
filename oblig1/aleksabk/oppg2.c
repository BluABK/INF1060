#include <stdio.h>
#include <string.h>

void encode_file(char *input, char *output, const unsigned char *symbols) {
	int i;
	output = "";
	for (i = 0; input[i] != '\0'; i++) {
		switc


		if (strchr(' ', input[i])) {
			output = output + "00";
			continue;
		}
		if (strchr(':', input[i])) {
			output = output + "01";
			continue;
		}
		if (strchr('@', input[i])) {
			output = output + "10";
			continue;
		}
		if (strchr('\n', input[i])) {
			output = output + "11";
		}
		// If nothing matches, keep the character as-is.
		output = output + input[i];
	}
	printf("DEBUG:\tPrinting what-would-be output file:\n");
	printf("%s", output);
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

	if (file == NULL) {
		printf("Failed to open file '%s', did you make a typo?\n", filename);
		return NULL;
	}

	// Determine file size
	fseek(file, 0, SEEK_END);
	long f_size = ftell(file);
	fseek(file, 0, SEEK_SET);
	content = malloc(f_size);

	// Read file into data variable
	fread(content, f_size, 1, file);

	// Close the file handle
	fclose(file);

	// return the data variable
	return content;
}

int main(int argc, char *argv[]) {
	char *data;
	const unsigned char symbols[4] = {' ',':','@','\n'};

	// If an input file was specified
	if (argc == 3) 	data = read_file(argv[2]);

	// Check for arguments and run the appropriate function
	if 	(!strcmp(argv[1], "p") && argc > 2)		printf("%s\n", data);
	else if (!strcmp(argv[1], "e") && argc > 3)		encode_file(data, argv[3], symbols);
	else if (!strcmp(argv[1], "d") && argc > 3)		decode_file();
	else							help();

	return 0;
}
