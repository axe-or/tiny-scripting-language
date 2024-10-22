#pragma once

#include "prelude.h"

#ifdef assert
#undef assert
#endif

#include <stdio.h>

extern noreturn void abort();

static inline
void debug_assert_ex(bool predicate, cstring msg, cstring file, i32 line){
	if(!predicate){
		fprintf(stderr, "%s:%d Assertion failed: %s\n", file, line, msg);
		abort();
	}
}

#ifndef DISABLE_ASSERT
#define assert(expr, msg) debug_assert_ex((expr), msg, __FILE__, __LINE__)
#else
#define assert(Expr) ((void)(Expr))
#endif

