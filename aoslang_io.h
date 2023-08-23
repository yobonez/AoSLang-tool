#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <share.h>

//int check_header(FILE* langfile_ptr);
FILE* open_aoslang(const char* aoslang_filename);
int aoslang_read(FILE* langfile_ptr);
int aoslang_export(FILE* langfile_ptr);
void aoslang_pack();