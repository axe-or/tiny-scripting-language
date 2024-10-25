#pragma once
#include "prelude.h"

struct source_location {
	string file;
	i32 line;
	i32 column;
};

struct source_location src_loc_from_string_offset(string src, i64 offset){
	struct source_location loc = {
		.line = 1,
		.column = 0,
	};

	for(i64 i = 0; i < src.len; i += 1){
		if(src.data[i] == '\n'){
			loc.column = 0;
			loc.line += 1;
		} else {
			loc.column += 1;
		}
	}

	return loc;
}
