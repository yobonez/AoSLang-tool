#include <stdio.h>
#include <stdlib.h>
#include <share.h>
#include <string.h>

#include "aoslang_types.h"
#include "aoslang_io.h"

int main(int argc, char* argv[])
{	
	for(int i = 0; i < argc; i++)
	{ 
		if (argc < 3 || !*argv[i]) { printf("Usage: %s <export|read|pack> <filename> \n", argv[0]); return 1; }
	}

	const char* provided_filename = argv[2];

	if (strcmp(argv[1], "export") == 0) {
		FILE* aoslang_file = open_aoslang(provided_filename);

		if (aoslang_file == NULL || aoslang_export(aoslang_file) != 0)
		{ printf("An error occured.\n"); return -1; }
	}
	if (strcmp(argv[1], "read") == 0) {
		FILE* aoslang_file = open_aoslang(provided_filename);

		if (aoslang_file == NULL || aoslang_read(aoslang_file) != 0)
		{ printf("An error occured.\n"); return -1; }
	}
	if (strcmp(argv[1], "pack") == 0) {
		printf("\"pack\" Not implemented yet...");
		return -1;
	}

	printf("Done. Press any key to continue...");
	getchar();
	return 0;
}
