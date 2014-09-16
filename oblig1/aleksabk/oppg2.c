#include <stdio.h>  // Required for I/O functions
#include <string.h> // Required for str*
#include <stdlib.h> // Required for free()

void encode_file (char *input, char *outfile) {
	// Open file for binary writing
	FILE *outfd = fopen(outfile, "wb");
	int i, j;
	unsigned char outb, out;

	// If the file could not be opened
	if (outfd == NULL) {
		printf("Failed to open file '%s' for writing, did you make a typo?\n", outfile);
		return;
	}

	for (i = 0, j = 0; input[i]; i++) {
		if (input[i] == ' ')       out = 0b00;
		else if (input[i] == ':')  out = 0b01;
		else if (input[i] == '@')  out = 0b10;
		else if (input[i] == '\n') out = 0b11;
		else                       out = 0b00; // If there's no match, put space

		// Push amounts:
		// j	bits
		// 0	6	aa000000
		// 1	4	00bb0000
		// 2	2	0000cc00
		// 3	0	000000dd
		// 		--------
		// bitwise or:	aabbccdd
		outb |= out << (6 - (2 * j));

		if (j == 3) {
			// We have filled outb, now write it
			fwrite(&outb,1,1,outfd);
			outb=0;
			j=0;
		} else
			j++;
	}
	// TODO: if j != 0, then input size cannot be divided by 4, then we can either add this byte (add extra spaces) or do as we do now (drop it and truncate the file to nearest 4 byte border

	// Close the file handle
	fclose(outfd);
}

void decode_file (unsigned char *in, int length) {
	int i, j;
	const char map[4] = {' ', ':', '@', '\n'};
	unsigned char value;

	for (i = 0; i < length; i++) {
		// decode and print in[i]
		for (j = 0; j < 4; j++) {
			// push amounts:
			// j	bits	from		to
			// 0	6	aabbccdd	000000aa
			// 1	4	aabbccdd	0000aabb
			// 2	2	aabbccdd	00aabbcc
			// 3	0	aabbccdd	aabbccdd
			//
			// & 0b11
			// from		to
			// 000000aa	000000aa
			// 0000aabb	000000bb
			// 00aabbcc	000000cc
			// aabbccdd	000000dd
			value = (in[i] >> (6 - (2 * j))) & 0b11;
			putchar(map[value]);
		}
	}
	putchar('\n');
}

void help () {
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

// Writes the number of bytes read to the int that *length points to
char *read_file (char *filename, int *length) {
	// Open file for binary reading
	FILE *file = fopen(filename, "rb");
	char *content;

	// If the file could not be opened
	if (file == NULL) {
		printf("Failed to open file '%s', did you make a typo?\n", filename);
		return NULL;
	}

	// Determine file size
	fseek(file, 0, SEEK_END);
	long f_size = ftell(file);
	fseek(file, 0, SEEK_SET);
	content = malloc(f_size+1);
	// null termination
	content[f_size] = 0;

	*length = fread(content, 1, f_size, file);

	// Close the file handle
	fclose(file);

	return content;
}

int main (int argc, char *argv[]) {
	char *data = NULL;
	int length;

	// If an input file was specified
	if (argc >= 3) {
		data = read_file(argv[2], &length);
		if(data == NULL)
			return 1;
	}

	// Check for arguments and run the appropriate function
	if      (argc > 2 && !strcmp(argv[1], "p")) printf("%s\n", data);
	else if (argc > 3 && !strcmp(argv[1], "e")) encode_file(data, argv[3]);
	else if (argc > 2 && !strcmp(argv[1], "d")) decode_file((unsigned char *)data, length);
	else                                        help();

	if (data) free(data);

	return 0;
}
