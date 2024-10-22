#include "prelude.h"
#include "memory.c"
#include "token.c"
#include "string.c"
#include "assert.c"
#include <stdio.h>

#define KiB(x) ((isize)(x) * 1024ll)
#define MiB(x) ((isize)(x) * 1024ll * 1024ll)

static byte lexer_mem[MiB(16)] = {0};

int main(){
	struct arena lex_arena = arena_make(lexer_mem, sizeof(lexer_mem));
	struct token_array arr = {
		.data = NULL,
		.len = 0,
		.cap = 0,
		.arena = lex_arena,
	};

	for(int i = 0; i < 250000; i += 1){
		tk_array_push(&arr, (struct token){tk_paren_open});
	}
	printf("ARENA: +%ld/%ld ARR: len: %ld cap: %ld\n", arr.arena.offset, arr.arena.cap, arr.len, arr.cap);
	printf("Usage: %f\n", (f64)arr.arena.offset / (f64)arr.arena.cap);
	return 0;
}
