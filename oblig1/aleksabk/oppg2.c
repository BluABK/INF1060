#include <stdio.h>
#include <string.h>

void print_file() {
	// Dummy function
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

int main(int argc, char *argv[]) {
	FILE *fp;
	
	// Check for arguments and run the appropriate function
	if 	(!strcmp(argv[1], "p"))		print_file();
	else if (!strcmp(argv[1], "e"))		encode_file();
	else if (!strcmp(argv[1], "d"))		decode_file();
	else					help();

	return 0;
}
