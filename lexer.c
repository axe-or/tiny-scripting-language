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

struct token lex_comment(struct lexer* lex){}

#define TK(Name) ((struct token){.type = tk_##Name, .offset = lex->current})

#define TOKEN_1(Res) { return Res; }

#define TOKEN_2(Match1, Res1, Alt){ \
	if(lex_advance_matching(lex, Match1)) \
		return Res1; \
	else \
		return Alt; }

#define TOKEN_3(Match1, Res1, Match2, Res2, Alt){ \
	if(lex_advance_matching(lex, Match1)) \
		return Res1; \
	else if(lex_advance_matching(lex, Match2)) \
	  return Res2; \
	else \
	  return Alt; }

struct token lex_next(struct lexer* lex){
	struct token token = {0};

	byte c = lex_advance(lex);
	if(c == 0){
		token.type = tk_eof;
		return token;
	}

	switch(c){
		case '(': TOKEN_1(TK(paren_open));
		case ')': TOKEN_1(TK(paren_close));
		case '[': TOKEN_1(TK(square_open));
		case ']': TOKEN_1(TK(square_close));
		case '{': TOKEN_1(TK(curly_open));
		case '}': TOKEN_1(TK(curly_close));

		case '.': TOKEN_1(TK(dot));
		case ',': TOKEN_1(TK(comma));
		case ':': TOKEN_1(TK(colon));
		case ';': TOKEN_1(TK(semicolon));

		case '+': TOKEN_2('=', TK(assign_plus), TK(plus));
		case '-': TOKEN_2('=', TK(assign_minus), TK(minus));
		case '*': TOKEN_2('=', TK(assign_star), TK(star));
		case '%': TOKEN_2('=', TK(assign_modulo), TK(modulo));
		case '&': TOKEN_2('=', TK(assign_bit_and), TK(bit_and));
		case '|': TOKEN_2('=', TK(assign_bit_or), TK(bit_or));
		case '~': TOKEN_2('=', TK(assign_tilde), TK(tilde));
		case '/': TOKEN_3('/', lex_comment(lex), '=', TK(assign_slash), TK(slash));

		case '=': TOKEN_2('=', TK(eq), TK(assign));
		case '!': TOKEN_2('=', TK(neq), TK(not));
		case '>': TOKEN_3('=', TK(gteq), '>', TK(bit_shift_right), TK(gt));
		case '<': TOKEN_3('=', TK(lteq), '<', TK(bit_shift_left), TK(lt));
	}

	return token;
}

#undef TOKEN_3
#undef TOKEN_2
#undef TOKEN_1
#undef TK
