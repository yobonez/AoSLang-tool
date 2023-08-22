#define MAX_STR_LEN 256

typedef struct aoslang_string {
	int offset;
	char entry[MAX_STR_LEN];
} AosLangString ;