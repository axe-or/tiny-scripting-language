#pragma once

#include "memory.c"

#define TEMP_ARENA_BUF_SIZE MiB(64)

static struct arena* temp_arena(){
	static byte temp_arena_buf[TEMP_ARENA_BUF_SIZE];
	static struct arena temp_arena_arena = {
		.data = temp_arena_buf,
		.offset = 0,
		.cap = TEMP_ARENA_BUF_SIZE,
	};

	return &temp_arena_arena;
}

