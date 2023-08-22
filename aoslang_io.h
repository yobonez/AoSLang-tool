#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <share.h>

int check_header(FILE* langfile_ptr);
int aoslang_read(FILE* langfile_ptr, size_t string_count);
int aoslang_export(FILE* langfile_ptr, size_t string_count);
void aoslang_pack();