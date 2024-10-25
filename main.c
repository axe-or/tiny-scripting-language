#include "prelude.h"
#include "lexer.c"
#include "memory.c"
#include "token.c"
#include "assert.c"
#include "temp_arena.c"
#include <stdio.h>

static byte lexer_mem[MiB(16)];

#define FILE_MAX_PATH_ 4096
static string file_read_all_text(string path, struct arena* arena){
	string result = {0};
	byte buf [FILE_MAX_PATH_];
	mem_copy_no_overlap(buf, path.data, min(path.len, FILE_MAX_PATH_));
	buf[FILE_MAX_PATH_ - 1] = 0;

	FILE* f = fopen((cstring)buf, "rb");
	if(f == NULL){
		goto exit;
	}

	fseek(f, 0, SEEK_END);
	i64 end = ftell(f);
	rewind(f);
	i64 start = ftell(f);
	i64 size = end - start;

	byte* file_buf = arena_push_array(arena, byte, size);
	if(file_buf == NULL){
		goto exit;
	}
	fread(file_buf, 1, size, f);
	result.data = file_buf;
	result.len = size;

exit:
	if(f != NULL) fclose(f);
	return result;
}
#undef FILE_MAX_PATH_

int main(){
	struct arena main_arena = arena_make(lexer_mem, sizeof(lexer_mem));

	string src = file_read_all_text(str_lit("main.c"), &main_arena);
	printf("%.*s", str_fmt(src));
	return 0;
}
