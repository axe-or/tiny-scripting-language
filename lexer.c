#pragma once
#include "prelude.h"
#include "memory.c"
#include "location.c"
#include "token.c"

struct lexer {
	i32 current;
	i32 previous;
	string source;
	string file;
};

struct source_location lex_current_loc(struct lexer* lex){
	return src_location(lex->file, lex->current);
}

byte lex_advance(struct lexer* lex){
	if(lex->current >= lex->source.len){
		return 0;
	}

	lex->current += 1;
	return lex->source.data[lex->current - 1];
}

byte lex_peek(struct lexer* lex, i32 delta){
	i32 pos = lex->current + delta;
	if(pos < 0 || pos >= lex->source.len){
		return 0;
	}
	return lex->source.data[lex->current + delta];
}

bool lex_advance_matching(struct lexer* lex, byte match){
	if(lex->current >= lex->source.len){
		return false;
	}
	if(lex->source.data[lex->current] == match){
		lex->current += 1;
		return true;
	}
	return false;
}

#define ARITH_OR_ASSIGN(name) { \
	if(lex_advance_matching(lex, '=')) \
		token.type = tk_assign_##name; \
	else \
		token.type = tk_##name; \
	return token; }

#define BASIC_TOKEN(name) { \
	token.type = tk_##name; \
	return token; \
}

struct token lex_next(struct lexer* lex){
	struct token token = {0};

	byte c = lex_advance(lex);
	if(c == 0){
		token.type = tk_eof;
		return token;
	}

	switch(c){
		case '(': BASIC_TOKEN(paren_open);
		case ')': BASIC_TOKEN(paren_close);
		case '[': BASIC_TOKEN(square_open);
		case ']': BASIC_TOKEN(square_close);
		case '{': BASIC_TOKEN(curly_open);
		case '}': BASIC_TOKEN(curly_close);

		case '.': BASIC_TOKEN(dot);
		case ',': BASIC_TOKEN(comma);
		case ':': BASIC_TOKEN(colon);
		case ';': BASIC_TOKEN(semicolon);

		case '+': ARITH_OR_ASSIGN(plus);
		case '-': ARITH_OR_ASSIGN(minus);
		case '*': ARITH_OR_ASSIGN(star);
		case '%': ARITH_OR_ASSIGN(modulo);
		case '&': ARITH_OR_ASSIGN(bit_and);
		case '|': ARITH_OR_ASSIGN(bit_or);
		case '~': ARITH_OR_ASSIGN(tilde);

		case '>':
		case '<':

		case '/': /* TODO */;
	}

	return token;
}

#undef BASIC_TOKEN
#undef ARITH_OR_ASSIGN
