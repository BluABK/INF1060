#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void encode_file(char *input, char *outfile) {
	FILE *outfd = fopen(outfile, "wb");
	int i,j;
	unsigned char outb,out;

	if (outfd == NULL) {
		printf("Failed to open file '%s' for writing, did you make a typo?\n", outfile);
		return;
	}

	j=0;
	for (i = 0; input[i] != '\0'; i++) {
		if (input[i]==' ') {
			out = 0b00;
		} else if (input[i]==':') {
			out = 0b01;
		} else if (input[i]=='@') {
			out = 0b10;
		} else if (input[i]=='\n') {
			out = 0b11;
		} else {
			out = 0b00; // Default is space
		}

		// Push amounts:
		// j	bits
		// 0	6	aa000000
		// 1	4	00bb0000
		// 2	2	0000cc00
		// 3	0	000000dd
		// 		--------
		// bitwise or:	aabbccdd
		outb |= out << ((3-j)*2);

		if(j == 3){
			// We have filled outb, now write it
			fwrite(&outb,1,1,outfd);
			outb=0;
			j=0;
		} else {
			j++;
		}
	}

	// Close the file handle
	fclose(outfd);
}

void decode_file(unsigned char *in, int length) {
	int i,j;
	const char table[4] = {' ', ':', '@', '\n'};
	unsigned char value;

	for (i=0; i<length; i++) {
		// decode and print in[i]
		for(j=0;j<4;j++){
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
			value = (in[i]>>(6-(2*j))) & 0b11;
			putchar(table[value]);
		}
	}
	putchar('\n');
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

// take *length in order to "return" a second parameter
char *read_file(char *filename, int *length) {
	FILE *file = fopen(filename, "rb");
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
	// put f_size in count so that length is set to the number of bytes read
	*length = fread(content, 1, f_size, file);

	// Close the file handle
	fclose(file);

	// return the data variable
	return content;
}

int main(int argc, char *argv[]) {
	char *data=NULL;
	int length;

	if (argc < 2){
		help();
		return 0;
	}

	// If an input file was specified
	if (argc >= 3) {
		data = read_file(argv[2], &length);
		if(data == NULL)
			return 1;
	}

	// Check for arguments and run the appropriate function
	if 	(!strcmp(argv[1], "p") && argc > 2)		printf("%s\n", data);
	else if (!strcmp(argv[1], "e") && argc > 3)		encode_file(data, argv[3]);
	else if (!strcmp(argv[1], "d") && argc > 2)		decode_file((unsigned char *)data, length);
	else							help();

	if(data) free(data);
	return 0;
}
