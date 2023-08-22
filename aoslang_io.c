#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "aoslang_io.h"
#include "aoslang_types.h"

void aoslang_read(FILE* langfile_ptr)
{
}

int aoslang_export(FILE* langfile_ptr)
{
	// create destination file
	FILE* destination_file;
	if ((destination_file = _fsopen("AoSLangExport.txt", "wb", _SH_DENYWR)) == NULL)
	{
		printf("Error opening file: %s", "AoSLangExport.txt");
		return -1;
	}

	if (fseek(langfile_ptr, 0, SEEK_END) != 0) {
		printf("fseek error: (ferror: %i)", ferror(langfile_ptr));
		return -1;
	}

	size_t langfile_size = ftell(langfile_ptr);

	fseek(langfile_ptr, 0x180, SEEK_SET);
	size_t current_pos = ftell(langfile_ptr);

	char current_string[MAX_STR_LEN];

	size_t string_zero_pos = 0;
	size_t i = 0;
	while (i < (langfile_size - current_pos)) {
		for (i = 0; i < (langfile_size - current_pos); i++)
		{
			char c = fgetc(langfile_ptr);
			if (c != '\0') {
				current_string[i - string_zero_pos] = c;
			}
			else {
				current_string[i - string_zero_pos] = '\n'; // insert newline before \0
				current_string[i - string_zero_pos + 1] = c; // \0

				fwrite(current_string, sizeof(char), strlen(current_string), destination_file);
				current_string[0] = '\0';
				string_zero_pos = i + 1;
			}
		}
	}

	fclose(langfile_ptr);
	fclose(destination_file);
	
	return 0;
}

void aoslang_pack()
{
	// U can just count amount of strings, to determine how much file offsets to prepare.
	// Kinda obvious, but i write it here just in case i forget and make some weird overcomplicated ass implementation.
}
