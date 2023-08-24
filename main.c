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

	const char* mode = argv[1];
	const char* filename = argv[2];

	if (strcmp(mode, "export") == 0) {
		FILE* aoslang_file = open_aoslang(filename);

		if (aoslang_file == NULL || aoslang_export(aoslang_file, mode) != 0)
		{ printf("An error occured.\n"); return -1; }
	}
	if (strcmp(mode, "read") == 0) {
		FILE* aoslang_file = open_aoslang(filename);

		if (aoslang_file == NULL || aoslang_read(aoslang_file, mode) != 0)
		{ printf("An error occured.\n"); return -1; }
	}
	if (strcmp(mode, "pack") == 0) {
		printf("\"pack\" Not implemented yet...");
		return -1;
	}

	return 0;
}
