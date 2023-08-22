#include <stdio.h>
#include <stdlib.h>
#include <share.h>

#include "aoslang_types.h"
#include "aoslang_io.h"



int main(int argc, char* argv[])
{
	FILE* aoslang_file;

	if ((aoslang_file = _fsopen("AoSLang-EN.bin", "rb", _SH_DENYWR)) == NULL)
	{
		printf("Error opening file: %s", "AoSLang-EN.bin");
		return -1;
	}

	if (aoslang_export(aoslang_file) != 0) {
		printf("An error occured:\n");
		return -1;
	}

	printf("Press any key to continue...");
	getchar();
	return 0;
}
