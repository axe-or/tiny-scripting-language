#pragma once
#include "prelude.h"
#include "memory.c"

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
	X("<<", bit_shift_left) \
	X(">>", bit_shift_right) \
	/* Assignment versions */ \
	X("+=", assign_plus) \
	X("-=", assign_minus) \
	X("*=", assign_star) \
	X("/=", assign_slash) \
	X("%=", assign_modulo) \
	X("~=", assign_tilde) \
	X("&=", assign_bit_and) \
	X("|=", assign_bit_or) \
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
	X("struct", struct) \
	X("fun", fun) \
	X("return", return) \
	X("if", if) \
	X("else", else) \
	X("for", for) \
	X("continue", continue) \
	X("break", break) \
	/* Control & Errors */ \
	X("<EOF>", eof)

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
	u32 offset;
	string lexeme;
	union {
		i64 integer;
		f64 real;
		string str;
	} value;
};

struct token_array {
	struct token* data;
	isize len;
	isize cap;

	struct arena arena;
};

void tk_array_push(struct token_array* arr, struct token val){
	if(arr->len >= arr->cap){
		isize new_cap = mem_align_forward(arr->len * 2 + 1, 4);
		isize new_size = new_cap * sizeof(val);

		void* new_data = arena_resize(&arr->arena, arr->data, new_size);
		if(new_data == NULL){
			new_data = arena_alloc(&arr->arena, new_size, alignof(struct token));
			assert(new_data != NULL, "Failed to expand token array");
			mem_copy_no_overlap(&new_data, arr->data, arr->len * sizeof(*arr->data));
		}
		arr->data = new_data;
		arr->cap = new_cap;
	}

	arr->data[arr->len] = val;
	arr->len += 1;
}


