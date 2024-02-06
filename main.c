#include <stdlib.h>
#include <string.h>

#include "aoslang_types.h"
#include "aoslang_io.h"

int main(int argc, char* argv[])
{	
	for(int i = 0; i < argc; i++)
	{ 
		if (argc < 3 || !*argv[i]) { printf("Usage: %s <export|read|pack> <filename> [out filename]\n<> - required, \n[] - optional - for \"export\" & \"pack\"\n", argv[0]); return 1; }
	}

	const char* mode = argv[1];
	const char* filename = argv[2];

	if (strcmp(mode, "export") == 0) {
		char outfilename[_MAX_PATH] = "AoSLangExport.txt";
		if (argv[3]) { strcpy_s(outfilename, sizeof(outfilename), argv[3]); }

		FILE* aoslang_file = aoslang_open(filename);
		if (aoslang_file == NULL || aoslang_export(aoslang_file, outfilename, mode) != 0) { printf("An error occured.\n"); return -1; }
	}
	if (strcmp(mode, "read") == 0) {
		FILE* aoslang_file = aoslang_open(filename);
		if (aoslang_file == NULL || aoslang_read(aoslang_file, mode) != 0) { printf("An error occured.\n"); return -1; }
	}
	if (strcmp(mode, "pack") == 0) {
		char outfilename[_MAX_PATH] = "AoSLang-PACKED.bin";
		if (argv[3]) { strcpy_s(outfilename, sizeof(outfilename), argv[3]); }

		if (aoslang_pack(filename, outfilename, mode) != 0) { printf("An error occured.\n"); return -1; }
	}

	return 0;
}
