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

	const char* aoslang_filename = argv[2];
	size_t amount_of_strings = 94;                            // default amount of strings - i will probably make this modifiable in the future 
	                                                          // for example for some patched clients that maybe have more/less of them

	size_t strings_location = amount_of_strings * 4 + 8;      // +8 -> skip over 4 byte header and 4 bytes of unknown data

	FILE* aoslang_file;

	if ((aoslang_file = _fsopen(aoslang_filename, "rb", _SH_DENYWR)) == NULL)
	{ printf("Error opening file: %s", aoslang_filename); return -1; }

	if (check_header(aoslang_file) != 0)
	{ printf("The selected file is not a valid AoSLang file or is corrupted."); return -1; }

	if (strcmp(argv[1], "export") == 0) {
		if (aoslang_export(aoslang_file, strings_location) != 0)
		{ printf("An error occured:\n"); return -1; }
	}
	if (strcmp(argv[1], "read") == 0) {
		printf("\"read\" Not implemented yet...");
		return -1;
	}
	if (strcmp(argv[1], "pack") == 0) {
		printf("\"pack\" Not implemented yet...");
		return -1;
	}
	
	//if (aoslang_read(aoslang_file, amount_of_strings) != 0) {
	//	printf("An error occured:\n");
	//	return -1;
	//}

	printf("Done. Press any key to continue...");
	getchar();
	return 0;
}
