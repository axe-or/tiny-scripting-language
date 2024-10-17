#pragma once
#include "prelude.h"
#include "assert.c"

#if defined(__clang__) || defined(__GNUC__)
#define _memset_impl __builtin_memset
#define _memcpy_impl __builtin_memcpy
#define _memmove_impl __builtin_memmove
#else
extern void* memset(void *dst, int c, size_t n);
extern void* memmove(void *dst, void const * src, size_t n);
extern void* memcpy(void *dst, void const * src, size_t n);

#define _memset_impl memset
#define _memcpy_impl memcpy
#define _memmove_impl memmove
#endif

void mem_copy(void* dest, void* source, isize nbytes){
	assert(nbytes >= 0);
	_memmove_impl(dest, source, nbytes);
}

void mem_copy_no_overlap(void* dest, void* source, isize nbytes){
	assert(nbytes >= 0);
	_memcpy_impl(dest, source, nbytes);
}

void mem_set(void* dest, byte val, isize nbytes){
	assert(nbytes >= 0);
	_memset_impl(dest, val, nbytes);
}

#undef _memset_impl
#undef _memcpy_impl
#undef _memmove_impl

bool mem_valid_alignment(isize align){
	return (align & (align - 1)) == 0 && (align != 0);
}

uintptr mem_align_forward(uintptr p, uintptr a){
	assert(mem_valid_alignment(a));
	uintptr mod = p & (a - 1);
	if(mod > 0){
		p += (a - mod);
	}
	return p;
}

struct arena {
	byte* data;
	isize offset;
	isize cap;
};

uintptr arena_required_mem(uintptr cur, isize nbytes, isize align){
	assert(mem_valid_alignment(align));
	uintptr aligned  = mem_align_forward(cur, align);
	uintptr padding  = (uintptr)(aligned - cur);
	uintptr required = padding + nbytes;
	return required;
}

void* arena_alloc_non_zero(struct arena* a, isize size, isize align){
	uintptr base = (uintptr)a->data;
	uintptr current = (uintptr)base + (uintptr)a->offset;

	uintptr available = (uintptr)a->cap - (current - base);
	uintptr required = arena_required_mem(current, size, align);

	if(required > available){
		return NULL;
	}

	a->offset += required;
	void* allocation = &a->data[a->offset - size];
	return allocation;
}

void* arena_alloc(struct arena* a, isize size, isize align){
	void* p = arena_alloc_non_zero(a, size, align);
	if(p){
		mem_set(p, 0, size);
	}
	return p;
}

void arena_free(struct arena* a){
	a->offset = 0;
}


