#include "prelude.h"
#include "memory.c"
#include "token.c"
#include "string.c"
#include "assert.c"
#include <stdio.h>

static byte lexer_mem[MiB(16)];

static byte scratch_mem[MiB(64)];

int main(){
	struct arena scratch_arena = {
		.data = scratch_mem,
		.cap = sizeof(scratch_mem),
		.offset = 0,
	};

	struct error_list list = {
		.head = NULL,
		.arena = scratch_arena,
	};

	error_emit(&list,
			src_loc(str_lit("main.c"), 100),
			lexer_err_unexpected_char,
			"DAMN... %d", 6969);

	error_emit(&list,
			src_loc(str_lit("main.c"), 100),
			parser_err_prohibited_statement,
			"DAMN... %d", 100);

	error_print_list(list);

	return 0;
}
