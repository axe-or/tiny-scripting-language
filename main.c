#include "prelude.h"
#include "memory.c"
#include "string.c"
#include "assert.c"

#include <stdio.h>

struct lexer {
	i32 current;
	i32 previous;
	string source;
};

#define TOKENS \
	X("<Unknown>", unknown) \
	/* Delimiters */ \
	X("(", paren_open) \
	X(")", paren_close) \
	X("[", square_open) \
	X("]", square_close) \
	X("{", curly_open) \
	X("}", curly_close) \
	/* Punctuation */ \
	X(":", colon) \
	X(",", comma) \
	X(";", semicolon) \
	X("->", arrow_right) \
	/* Arithmetic & Bitwise */ \
	X("+", plus) \
	X("-", minus) \
	X("*", star) \
	X("/", slash) \
	X("%", modulo) \
	X("~", tilde) \
	X("&", bit_and) \
	X("|", bit_or) \
	/* Logic & Comparison */ \
	X("!", not) \
	X("&&", and) \
	X("||", or) \
	X("==", eq) \
	X("!=", neq) \
	X(">", gt) \
	X("<", lt) \
	X(">=", gteq) \
	X("<=", lteq) \
	/* Other operators */ \
	X(".", dot) \
	X("=", assign) \
	/* Special & Literals */ \
	X("<identifier>", identifier) \
	X("<string>", string) \
	X("<integer>", integer) \
	X("<real>", real) \
	X("true", true) \
	X("false", false) \
	X("nil", nil) \
	/* Keywords */ \
	X("var", var) \
	X("record", record) \
	X("fun", fun) \
	X("return", return) \
	X("if", if) \
	X("else", else) \
	X("for", for) \
	X("continue", continue) \
	X("break", break)

enum token_type {
	#define X(_, name) tk_##name,
	TOKENS
	#undef X
	tk__count,
};

static const struct { string key; i32 val; } token_str_map[] = {
#define X(str, name) {.key = str_lit(str), .val = tk_##name },
	TOKENS
	#undef X
};

struct token {
	enum token_type type;
	string lexeme;
	union {
		i64 integer;
		f64 real;
		string str;
	} payload;
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

struct token_array {
	struct token* data;
	isize len;
	isize cap;
};

int main(){
	return 0;
}
