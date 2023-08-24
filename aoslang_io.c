#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "aoslang_io.h"
#include "aoslang_types.h"

size_t aoslang_get_size(FILE* langfile_ptr)
{
	fseek(langfile_ptr, 0, SEEK_END);
	return ftell(langfile_ptr);
}

size_t aoslang_get_first_offset(FILE* langfile_ptr) {
	fseek(langfile_ptr, 8, SEEK_SET); // skip 4 byte header and 4 byte unknown data
	size_t str_loc = 0;
	fread(&str_loc, sizeof(int), 1, langfile_ptr);

	return str_loc;
}

int check_header(FILE* langfile_ptr)
{
	char header[5];

	if (fgets(header, 5, langfile_ptr) == NULL) { printf("Error occured while trying to check the header."); return -1; }
	if (strcmp(header, "STR0") != 0) { return -1; }
	rewind(langfile_ptr);

	return 0;
}

FILE* open_aoslang(const char* aoslang_filename)
{
	FILE* aoslang_file;

	if ((aoslang_file = _fsopen(aoslang_filename, "rb", _SH_DENYWR)) == NULL) { printf("Error opening file: %s", aoslang_filename); return NULL; }

	if (check_header(aoslang_file) != 0) { printf("The selected file is not a valid AoSLang file or is corrupted."); return NULL; }

	return aoslang_file;
}


void get_next_string(FILE* fptr, char* buffer, size_t until_EOF, size_t* i, size_t zero_align, const char* mode)
{
	for (; *i < until_EOF; (*i)++)
	{
		char c = fgetc(fptr);
		if (c != '\0'){
			buffer[*i - zero_align] = c;
		}
		else {
			if (strcmp(mode, "read") == 0) {
				buffer[*i - zero_align] = c;
				break;
			}
			buffer[*i - zero_align] = '\n';           // insert newline before \0
			buffer[*i + 1 - zero_align] = c;          // \0
			(*i)++;                                   // when breaking the i still needs to be incremented
			break;
		}
		if (c == '\n' && strcmp(mode, "export") == 0) buffer[*i - zero_align] = '`'; // some strings already have newlines in them, i have to preserve them somehow for later
	}
}


int aoslang_read(FILE* langfile_ptr, const char* mode)
{
	size_t strings_loc = aoslang_get_first_offset(langfile_ptr);
	size_t strings_amount = (strings_loc - 8) / 4;

	fseek(langfile_ptr, 8, SEEK_SET); // jump over 4 byte header and 4 bytes of unknown data
	AosLangEntry* entries = (AosLangEntry*)malloc(sizeof(AosLangEntry) * strings_amount);

	int* offsets = (int*)malloc(sizeof(int) * strings_amount);

	if (entries == NULL || offsets == NULL) { printf("malloc error");  return -1; }

	for (size_t i = 0; i < strings_amount; i++)
	{
		int offset = 0;
		fread(&offset, sizeof(int), 1, langfile_ptr);
		offsets[i] = offset;
	}


	AosLangEntry entry = { 0 };
	size_t byte_counter = 0;
	size_t until_EOF = aoslang_get_size(langfile_ptr) - strings_loc;
	fseek(langfile_ptr, strings_loc, SEEK_SET);
	size_t zero_align = 0;

	for (size_t i = 0; i < strings_amount; i++)
	{
		entry.offset = offsets[i];


		char lang_string[MAX_STR_LEN] = { 0 };
		get_next_string(langfile_ptr, lang_string, until_EOF, &byte_counter, zero_align, mode);
		strcpy_s(entry.lang_string, sizeof(lang_string), lang_string); 

		memcpy(&entries[i], &entry, sizeof(AosLangEntry));
		zero_align = byte_counter;
	}

	printf("-----------------------------------------------------------------------\n");
	for (size_t i = 0; i < strings_amount; i++)
	{
		printf("Offset: 0x%x\nContent: %s\n", entries[i].offset, entries[i].lang_string);
		printf("-----------------------------------------------------------------------\n");
	}

	free(offsets);
	free(entries);
	offsets = NULL;
	entries = NULL;
	return 0;
}

int aoslang_export(FILE* langfile_ptr, const char* mode)
{
	// determine amount of strings in lang file and its first string offset
	size_t strings_loc = aoslang_get_first_offset(langfile_ptr);
	size_t amount_of_strings = (strings_loc - 8) / 4;

	FILE* destination_file;
	if ((destination_file = _fsopen("AoSLangExport.txt", "wb", _SH_DENYWR)) == NULL) {
		printf("Error opening file: %s", "AoSLangExport.txt");
		return -1;
	}

	size_t langfile_size = aoslang_get_size(langfile_ptr);

	fseek(langfile_ptr, strings_loc, SEEK_SET);
	size_t file_cursor_pos = ftell(langfile_ptr);

	char string_to_write[MAX_STR_LEN];
	size_t i = 0;
	size_t zero_align = 0;
	size_t chars_to_EOF = langfile_size - file_cursor_pos;
	printf("Exporting %d strings...\n", amount_of_strings);
	while (i < chars_to_EOF) {
		get_next_string(langfile_ptr, string_to_write, chars_to_EOF, &i, zero_align, mode);
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
	//char header_and_unknown[8] = "STR0_\0\0\0";
}
