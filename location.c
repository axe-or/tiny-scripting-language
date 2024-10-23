#include "prelude.h"
#include "string.c"

struct source_location {
	string file;
	i32 offset;
};

struct source_visual_location {
	string file;
	i32 line;
	i32 column;
};

struct source_location src_location(string file, i32 offset){
	struct source_location loc;
	loc.file = file;
	loc.offset = offset;
	return loc;
};

/*
struct source_visual_location src_to_visual_location_f(FILE* f, struct source_location loc){
	struct source_visual_location vloc = {
		.file = loc.file,
		.line = 1,
		.column = 0,
	};

	for(char c = fgetc(f); c != EOF; c = fgetc(f)){
		if(c == '\n'){
			vloc.line += 1;
			vloc.column = 0;
		} else {
			vloc.column += 1;
		}
	}
	return vloc;
}
*/

struct source_visual_location src_to_visual_location_s(string s, struct source_location loc){
	struct source_visual_location vloc = {
		.file = loc.file,
		.line = 1,
		.column = 0,
	};

	for(isize i = 0; i < s.len; i += 1){
		byte c = s.data[i];
		if(c == '\n'){
			vloc.line += 1;
			vloc.column = 0;
		} else {
			vloc.column += 1;
		}
	}
	return vloc;
}
