#pragma once
#include "prelude.h"
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

static inline
bool is_alpha(byte c){
	return ((c >= 'A') && (c <= 'Z'))
		|| ((c >= 'a') && (c <= 'z'));
}

static inline
bool is_decimal_digit(byte c){
	return (c >= '0') && (c <= '9');
}

static inline
bool is_hex_digit(byte c){
	return ((c >= '0') && (c <= '9'))
		|| ((c >= 'a') && (c <= 'f'))
		|| ((c >= 'A') && (c <= 'F'));
}

static inline
bool is_oct_digit(byte c){
	return ((c >= '0') && (c <= '9'))
		|| ((c >= 'a') && (c <= 'f'))
		|| ((c >= 'A') && (c <= 'F'));
}

string lex_current_lexeme(struct lexer* lex){
	return str_sub(lex->source, lex->previous, lex->current - lex->previous);
}

static const struct { string key; i32 val; } token_keyword_map[] = {
#define X(str, name) {.key = str_lit(str), .val = tk_##name },
	TOKEN_KEYWORDS()
	#undef X
};

struct token lex_comment(struct lexer* lex){
	lex->previous = lex->current;
	while(true){
		byte c = lex_advance(lex);
		if(c == '\n' || c == 0){
			break;
		}
	}

	return (struct token){
		.lexeme = lex_current_lexeme(lex),
		.type = tk_comment,
		.offset = lex->current,
	};
}

struct token lex_identifier(struct lexer* lex){
	lex->previous = lex->current;
	while(true){
		byte c = lex_advance(lex);
		if(!is_alpha(c) && !is_decimal_digit(c) && c != '_'){
			lex->current -= 1;
			break;
		}
	}

	struct token tk = {
		.lexeme = lex_current_lexeme(lex),
		.offset = lex->current,
		.type = tk_identifier,
	};

	for(isize i = 0; i < (sizeof(token_keyword_map) / sizeof(token_keyword_map[0])); i += 1){
		i32 type = token_str_map[i].val;
		if(str_eq(tk.lexeme, token_str_map[i].key)){
			tk.type = type;
		}
	}

	return tk;
}

struct token lex_number(struct lexer* lex){
	panic("Unimplemented");
}

struct token lex_rune(struct lexer* lex){
	panic("Unimplemented");
}

struct token lex_string(struct lexer* lex){
	panic("Unimplemented");
}

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

		default: {
			if(is_decimal_digit(c)){
				lex->current -= 1;
				return lex_number(lex);
			}
			else if(is_alpha(c) || c == '_'){
				lex->current -= 1;
				return lex_identifier(lex);
			}
			else if(c == '"'){
				return lex_string(lex);
			}
			else if(c == '\''){
				return lex_rune(lex);
			}
			else {
				token.type = tk_unknown;
				return token;
			}
		}
	}

	return token;
}

#undef TOKEN_3
#undef TOKEN_2
#undef TOKEN_1
#undef TK
