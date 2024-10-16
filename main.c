#include "prelude.h"
#include "string.c"

#include <stdio.h>

struct lexer {
	i32 current;
	i32 previous;

	byte const* source;
	i32 source_len;
};

enum token_type {
	tk_paren_open,
	tk_paren_close,
	tk_square_open,
	tk_square_close,
	tk_curly_open,
	tk_curly_close,
};

struct token {
	enum token_type type;
	byte const* lexeme;
};

byte lex_advance(struct lexer* lex){
	if(lex->current >= lex->source_len){
		return 0;
	}

	lex->current += 1;
	return lex->source[lex->current - 1];
}

byte lex_peek(struct lexer* lex, i32 delta){
	i32 pos = lex->current + delta;
	if(pos < 0 || pos >= lex->source_len){
		return 0;
	}
	return lex->source[lex->current + delta];
}


int main(){
	return 0;
}
