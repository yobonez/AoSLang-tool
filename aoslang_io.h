#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <share.h>

void aoslang_read(FILE* langfile_ptr);
int aoslang_export(FILE* langfile_ptr);
void aoslang_pack();