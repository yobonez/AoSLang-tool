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

	if (fgets(header, 5, langfile_ptr) == NULL) { printf("Error occured while trying to check the header.\n"); return -1; }
	if (strcmp(header, "STR0") != 0) { return -1; }
	rewind(langfile_ptr);

	return 0;
}

FILE* aoslang_open(const char* filename)
{
	FILE* aoslang_file;

	if ((aoslang_file = _fsopen(filename, "rb", _SH_DENYWR)) == NULL) { printf("Error opening file: %s\n", filename); return NULL; }
	if (check_header(aoslang_file) != 0) { printf("The selected file is not a valid AoSLang file or is corrupted.\n"); return NULL; }

	return aoslang_file;
}


void get_next_string(FILE* fptr, char* buffer, size_t until_EOF, size_t* i, size_t zero_align, const char* mode)
{
	for (; *i < until_EOF; (*i)++)
	{
		char c = fgetc(fptr);
		// replace \n with \0 when packing
		if (c == '\n' && strcmp(mode, "pack") == 0) {
			buffer[*i - zero_align] = '\0';   
			break;
		}
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
		if (c == '`' && strcmp(mode, "pack") == 0) buffer[*i - zero_align] = '\n';   // bring back newlines that would be intentionally put in aoslang file
	}
}


int aoslang_read(FILE* langfile_ptr, const char* mode)
{
	size_t strings_loc = aoslang_get_first_offset(langfile_ptr);
	size_t strings_amount = (strings_loc - 8) / 4;

	fseek(langfile_ptr, 8, SEEK_SET); // jump over 4 byte header and 4 bytes of unknown data
	AosLangEntry* entries = (AosLangEntry*)malloc(sizeof(AosLangEntry) * strings_amount);

	int* offsets = (int*)malloc(sizeof(int) * strings_amount);

	if (entries == NULL || offsets == NULL) { printf("malloc error\n");  return -1; }

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

	fclose(langfile_ptr);
	free(offsets);
	free(entries);
	offsets = NULL;
	entries = NULL;
	return 0;
}

int aoslang_export(FILE* langfile_ptr, const char* outfilename, const char* mode)
{
	// determine amount of strings in lang file and its first string offset
	size_t strings_loc = aoslang_get_first_offset(langfile_ptr);
	size_t amount_of_strings = (strings_loc - 8) / 4;

	FILE* destination_file;
	if ((destination_file = _fsopen(outfilename, "wb", _SH_DENYWR)) == NULL) {
		printf("Error opening file: %s\n", outfilename);
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
	
	printf("Done.\n");
	return 0;
}

int aoslang_pack(const char* filename, const char* outfilename, const char* mode)
{
	size_t strings_amount = 0;
	char lang_string[MAX_STR_LEN] = { 0 };

	FILE* strings_file;
	if ((strings_file = _fsopen(filename, "rt", SH_DENYWR)) == NULL) { printf("Error opening file: %s\n", filename); return -1; }

	while (fgets(lang_string, MAX_STR_LEN, strings_file) != NULL) {
		strings_amount++;
	}

	size_t strings_desired_loc = strings_amount * 4 + 8; // where the actual strings start in the file (after offsets)
	rewind(strings_file);

	AosLangEntry* entries = (AosLangEntry*)malloc(sizeof(AosLangEntry) * strings_amount);
	if (entries == NULL) { printf("malloc/memset error\n"); return -1; }

	size_t str_counter = 0;
	int current_offset = strings_desired_loc;

	size_t until_EOF = aoslang_get_size(strings_file);
	rewind(strings_file);
	size_t i = 0;
	size_t zero_align = 0;
	while (str_counter < strings_amount) 
	{
		get_next_string(strings_file, lang_string, until_EOF, &i, zero_align, mode);

		AosLangEntry entry = { 0 };
		entry.offset = strings_desired_loc;
		strcpy_s(entry.lang_string, sizeof(lang_string), lang_string);
		memcpy(&entries[str_counter], &entry, sizeof(AosLangEntry));

		strings_desired_loc += strlen(lang_string) + 1;
		str_counter++;
		zero_align = i;
	}

	// write
	FILE* out_aoslang;
	if ((out_aoslang = _fsopen(outfilename, "wb", SH_DENYWR)) == NULL) { printf("Error opening file: %s\n", outfilename); return -1; }

	fwrite("STR0_\0\0\0", sizeof(char), 8, out_aoslang); // write 4 byte header and 4 bytes of unknown data

	printf("Writing offsets...\n");
	for (size_t i = 0; i < strings_amount; i++)
	{
		fwrite(&entries[i].offset, sizeof(int), 1, out_aoslang);
	}
	
	printf("Writing strings...\n");
	fseek(out_aoslang, entries[0].offset, SEEK_SET);
	for (size_t i = 0; i < strings_amount; i++)
	{
		size_t str_size = strlen(entries[i].lang_string);
		int result = fwrite(entries[i].lang_string, sizeof(char), str_size + 1, out_aoslang);
	}

	fclose(strings_file);
	fclose(out_aoslang);
	free(entries);
	entries = NULL;

	printf("Done.\n");
	return 0;
}
