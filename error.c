#pragma once

#include "prelude.h"
#include "location.c"

enum compiler_error {
	comp_err_none = 0,

	lexer_err_unexpected_char,
	lexer_err_unclosed_string,
	lexer_err_invalid_escape_sequence,

	parser_err_unexpected_token,
	parser_err_prohibited_statement,

	checker_err_incompatible_types,
};

static inline
cstring compile_error_stage(enum compiler_error e){
	switch(e){
		case lexer_err_unexpected_char: return "Lexer";
		case lexer_err_unclosed_string: return "Lexer";
		case lexer_err_invalid_escape_sequence: return "Lexer";

		case parser_err_unexpected_token: return "Parser";
		case parser_err_prohibited_statement: return "Parser";

		case checker_err_incompatible_types: return "Checker";
		default: return "<Unknown>";
	}
}

struct error_list_entry {
	string message;
	struct source_location location;
	i32 error_kind;
	struct error_list_entry* next;
};

struct error_list {
	struct error_list_entry* head;
	struct arena arena;
};

#define MAX_ERROR_MSG_LEN KiB(8)

i32 error_emit(struct error_list* list, struct source_location location, i32 kind, cstring fmt, ...){
	static byte buf[MAX_ERROR_MSG_LEN];
	i32 n = 0;
	/* Format message */ {
		va_list args;
		va_start(args, fmt);
		n = vsnprintf((char*)buf, MAX_ERROR_MSG_LEN - 1, fmt, args);
		va_end(args);
	}

	byte* msg = arena_alloc(&list->arena, n, alignof(byte));
	struct error_list_entry* err = arena_push(&list->arena, struct error_list_entry);

	if(msg != NULL && err != NULL){
		mem_copy_no_overlap(msg, buf, n);
		struct error_list_entry* old = list->head;
		list->head = err;

		err->next = old;
		err->message = str_from_bytes(msg, n);
		err->error_kind = kind;
		err->location = location;
	}

	return kind;
}

#define TERM_RED "\x27[1;31m"
#define TERM_RESET "\x27[0m"

void error_print_list(struct error_list list){
	for(struct error_list_entry* cur = list.head;
		cur != NULL;
		cur = cur->next)
	{
		cstring err_stage = compile_error_stage(cur->error_kind);
		printf("["TERM_RED "%s error" TERM_RESET" %.*s:%d] %.*s\n",
				err_stage,
				str_fmt(cur->location.file),
				cur->location.offset,
				str_fmt(cur->message));
	}
}

#undef ERR_HEADER

