#pragma once
#include <stdio.h>

//int check_header(FILE* langfile_ptr);
FILE* aoslang_open(const char* aoslang_filename);
int aoslang_read(FILE* langfile_ptr, const char* mode);
int aoslang_export(FILE* langfile_ptr, const char* outfilename, const char* mode);
int aoslang_pack(const char* filename, const char* outfilename, const char* mode);