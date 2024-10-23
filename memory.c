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
	assert(nbytes >= 0, "Cannot copy < 0 bytes");
	_memmove_impl(dest, source, nbytes);
}

void mem_copy_no_overlap(void* dest, void* source, isize nbytes){
	assert(nbytes >= 0, "Cannot copy < 0 bytes");
	_memcpy_impl(dest, source, nbytes);
}

void mem_set(void* dest, byte val, isize nbytes){
	assert(nbytes >= 0, "Cannot copy < 0 bytes");
	_memset_impl(dest, val, nbytes);
}

#undef _memset_impl
#undef _memcpy_impl
#undef _memmove_impl

#define KiB(x) ((isize)(x) * 1024ll)
#define MiB(x) ((isize)(x) * 1024ll * 1024ll)
#define GiB(x) ((isize)(x) * 1024ll * 1024ll * 1024ll)

bool mem_valid_alignment(isize align){
	return (align & (align - 1)) == 0 && (align != 0);
}

uintptr mem_align_forward(uintptr p, uintptr a){
	assert(mem_valid_alignment(a), "Invalid memory alignment");
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

	void* last_allocation;
};

struct arena arena_make(byte* data, isize data_len){
	assert(data_len > 0, "Data length cannot be <= 0");
	struct arena a = {
		.data = data,
		.offset = 0,
		.cap = data_len,
		.last_allocation = NULL,
	};
	return a;
}

uintptr arena_required_mem(uintptr cur, isize nbytes, isize align){
	assert(mem_valid_alignment(align), "Invalid memory alignment");
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
	a->last_allocation = allocation;
	return allocation;
}

void* arena_alloc(struct arena* a, isize size, isize align){
	void* p = arena_alloc_non_zero(a, size, align);
	if(p){
		mem_set(p, 0, size);
	}
	return p;
}

#define arena_push(ArenaPtr, Type) \
	arena_alloc((ArenaPtr), sizeof(Type), alignof(Type))

#define arena_push_array(ArenaPtr, Type, Count) \
	arena_alloc((ArenaPtr), sizeof(Type) * (Count), alignof(Type))

void arena_free(struct arena* a){
	a->offset = 0;
	a->last_allocation = NULL;
}

// Resize allocation in-place
void* arena_resize(struct arena* a, void* p, isize size){
	if(p != a->last_allocation || size < 0 || p == NULL){
		return NULL;
	}

	uintptr base = (uintptr)a->data;
	uintptr last_offset = (uintptr)p - base;

	isize old_size = a->offset - last_offset;
	isize delta = size - old_size;

	if((a->offset + delta) < a->cap){
		a->offset += delta;
	}
	else {
		return NULL;
	}

	return p;
}

