#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <share.h>

//int check_header(FILE* langfile_ptr);
FILE* open_aoslang(const char* aoslang_filename);
int aoslang_read(FILE* langfile_ptr, const char* mode);
int aoslang_export(FILE* langfile_ptr, const char* mode);
void aoslang_pack();