#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "aoslang_io.h"
#include "aoslang_types.h"

int check_header(FILE* langfile_ptr)
{
	char header[5];

	if (fgets(header, 5, langfile_ptr) == NULL) { printf("Error occured while trying to check the header."); return -1; };
	if (strcmp(header, "STR0") != 0)            { return -1; }
	rewind(langfile_ptr);

	return 0;
}

void get_next_string(FILE* fptr, char* buffer, size_t until_EOF, size_t* i, size_t zero_align)
{
	for (; *i < until_EOF; (*i)++)
	{
		char c = fgetc(fptr);
		if (c != '\0'){
			buffer[*i - zero_align] = c;
		}
		else {
			buffer[*i - zero_align] = '\n';           // insert newline before \0
			buffer[*i + 1 - zero_align] = c;          // \0
			(*i)++;                                   // when breaking the i still needs to be incremented
			break;
		}
		if (c == '\n') buffer[*i - zero_align] = '`'; // some strings already have newlines in them, i have to preserve them somehow for later
	}
}


int aoslang_read(FILE* langfile_ptr, size_t string_count)
{
	//AosLangEntry current_string = { 0 };
}

// will need to detect string amount automatically (i think its doable)
int aoslang_export(FILE* langfile_ptr, size_t string_count)
{
	FILE* destination_file;
	if ((destination_file = _fsopen("AoSLangExport.txt", "wb", _SH_DENYWR)) == NULL) {
		printf("Error opening file: %s", "AoSLangExport.txt");
		return -1;
	}

	if (fseek(langfile_ptr, 0, SEEK_END) != 0) {
		printf("fseek error: (ferror: %i)", ferror(langfile_ptr));
		return -1;
	}

	size_t langfile_size = ftell(langfile_ptr);

	fseek(langfile_ptr, string_count, SEEK_SET);
	size_t file_cursor_pos = ftell(langfile_ptr);

	char string_to_write[MAX_STR_LEN];
	size_t i = 0;
	size_t zero_align = 0;
	size_t chars_to_EOF = langfile_size - file_cursor_pos;

	while (i < chars_to_EOF) {
		get_next_string(langfile_ptr, string_to_write, chars_to_EOF, &i, zero_align);
		fwrite(string_to_write, sizeof(char), strlen(string_to_write), destination_file);
		string_to_write[0] = '\0';
		zero_align = i;
	}

	fclose(langfile_ptr);
	fclose(destination_file);
	
	return 0;
}

void aoslang_pack()
{
}
