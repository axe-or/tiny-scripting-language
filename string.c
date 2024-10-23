#pragma once

#include "prelude.h"

struct utf8_encode_result {
	byte bytes[4];
	i8 len;
};

struct utf8_decode_result {
	rune codepoint;
	i8 len;
};

static const rune UTF8_ERROR = 0xfffd;

static const struct utf8_encode_result UTF8_ERROR_ENCODED = {
	.bytes = {0xef, 0xbf, 0xbd},
	.len = 3,
};

struct utf8_iterator {
	byte const* data;
	isize data_length;
	isize current;
};

#define str_lit(cstr_lit) ((string){.data = (byte const*)(cstr_lit), .len = sizeof(cstr_lit)})

// Meant to be used with `%.*s`
#define str_fmt(str) str.len, str.data

#define SURROGATE1 ((i32)0xd800)
#define SURROGATE2 ((i32)0xdfff)

#define MASK2 (0x1f) /* 0001_1111 */
#define MASK3 (0x0f) /* 0000_1111 */
#define MASK4 (0x07) /* 0000_0111 */

#define MASKX (0x3f) /* 0011_1111 */

#define SIZE2 (0xc0) /* 110x_xxxx */
#define SIZE3 (0xe0) /* 1110_xxxx */
#define SIZE4 (0xf0) /* 1111_0xxx */

#define CONT  (0x80)  /* 10xx_xxxx */

#define CONTINUATION1 (0x80)
#define CONTINUATION2 (0xbf)

static inline
bool is_continuation_byte(rune c){
	return (c >= CONTINUATION1) && (c <= CONTINUATION2);
}

struct utf8_encode_result utf8_encode(rune c){
	struct utf8_encode_result res = {0};

	if((c >= CONTINUATION1 && c <= CONTINUATION2) ||
		(c >= SURROGATE1 && c <= SURROGATE2) ||
		(c > UTF8_RANGE4))
	{
		return UTF8_ERROR_ENCODED;
	}

	if(c <= UTF8_RANGE1){
		res.len = 1;
		res.bytes[0] = c;
	}
	else if(c <= UTF8_RANGE2){
		res.len = 2;
		res.bytes[0] = SIZE2 | ((c >> 6) & MASK2);
		res.bytes[1] = CONT  | ((c >> 0) & MASKX);
	}
	else if(c <= UTF8_RANGE3){
		res.len = 3;
		res.bytes[0] = SIZE3 | ((c >> 12) & MASK3);
		res.bytes[1] = CONT  | ((c >> 6) & MASKX);
		res.bytes[2] = CONT  | ((c >> 0) & MASKX);
	}
	else if(c <= UTF8_RANGE4){
		res.len = 4;
		res.bytes[0] = SIZE4 | ((c >> 18) & MASK4);
		res.bytes[1] = CONT  | ((c >> 12) & MASKX);
		res.bytes[2] = CONT  | ((c >> 6)  & MASKX);
		res.bytes[3] = CONT  | ((c >> 0)  & MASKX);
	}
	return res;
}

#define DECODE_ERROR ((struct utf8_decode_result){ .codepoint = UTF8_ERROR, .len = 0 })

struct utf8_decode_result utf8_decode(byte const* buf, isize len){
	struct utf8_decode_result res = {0};
	if(buf == NULL || len <= 0){ return DECODE_ERROR; }

	u8 first = buf[0];

	if((first & CONT) == 0){
		res.len = 1;
		res.codepoint |= first;
	}
	else if ((first & ~MASK2) == SIZE2 && len >= 2){
		res.len = 2;
		res.codepoint |= (buf[0] & MASK2) << 6;
		res.codepoint |= (buf[1] & MASKX) << 0;
	}
	else if ((first & ~MASK3) == SIZE3 && len >= 3){
		res.len = 3;
		res.codepoint |= (buf[0] & MASK3) << 12;
		res.codepoint |= (buf[1] & MASKX) << 6;
		res.codepoint |= (buf[2] & MASKX) << 0;
	}
	else if ((first & ~MASK4) == SIZE4 && len >= 4){
		res.len = 4;
		res.codepoint |= (buf[0] & MASK4) << 18;
		res.codepoint |= (buf[1] & MASKX) << 12;
		res.codepoint |= (buf[2] & MASKX) << 6;
		res.codepoint |= (buf[3] & MASKX) << 0;
	}
	else {
		return DECODE_ERROR;
	}

	// Validation
	if(res.codepoint >= SURROGATE1 && res.codepoint <= SURROGATE2){
		return DECODE_ERROR;
	}
	if(res.len > 1 && (buf[1] < CONTINUATION1 || buf[1] > CONTINUATION2)){
		return DECODE_ERROR;
	}
	if(res.len > 2 && (buf[2] < CONTINUATION1 || buf[2] > CONTINUATION2)){
		return DECODE_ERROR;
	}
	if(res.len > 3 && (buf[3] < CONTINUATION1 || buf[3] > CONTINUATION2)){
		return DECODE_ERROR;
	}

	return res;
}

// Steps iterator forward and puts rune and Length advanced into pointers,
// returns false when finished.
bool utf8_iter_next(struct utf8_iterator* iter, rune* r, i8* len){
	if(iter->current >= iter->data_length){ return 0; }

	struct utf8_decode_result res = utf8_decode(&iter->data[iter->current], iter->data_length);
	*r = res.codepoint;
	*len = res.len;

	if(res.codepoint == DECODE_ERROR.codepoint){
		*len = res.len + 1;
	}

	iter->current += res.len;

	return 1;
}

// Steps iterator backward and puts rune and its length into pointers,
// returns false when finished.
bool utf8_iter_prev(struct utf8_iterator* iter, rune* r, i8* len){
	if(iter->current <= 0){ return false; }

	iter->current -= 1;
	while(is_continuation_byte(iter->data[iter->current])){
		iter->current -= 1;
	}

	struct utf8_decode_result res = utf8_decode(&iter->data[iter->current], iter->data_length - iter->current);
	*r = res.codepoint;
	*len = res.len;
	return true;
}

struct string {
	isize len;
	byte const * data;
};

static inline
isize cstring_len(cstring cstr){
	static const isize CSTR_MAX_LENGTH = (~(u32)0) >> 1;
	isize size = 0;
	for(isize i = 0; i < CSTR_MAX_LENGTH && cstr[i] != 0; i += 1){
		size += 1;
	}
	return size;
}

static const string EMPTY = {0};

struct utf8_iterator str_iterator(string s){
	return (struct utf8_iterator){
		.current = 0,
		.data_length = s.len,
		.data = s.data,
	};
}

struct utf8_iterator str_iterator_reversed(string s){
	return (struct utf8_iterator){
		.current = s.len,
		.data_length = s.len,
		.data = s.data,
	};
}

bool str_empty(string s){
	return s.len == 0 || s.data == NULL;
}

string str_from(cstring data){
	string s = {
		.data = (byte const *)data,
		.len = cstring_len(data),
	};
	return s;
}

string str_from_bytes(byte const* data, isize length){
	string s = {
		.data = (byte const *)data,
		.len = length,
	};
	return s;
}

string str_from_range(cstring data, isize start, isize length){
	string s = {
		.data = (byte const *)&data[start],
		.len = length,
	};
	return s;
}

isize str_codepoint_count(string s){
	struct utf8_iterator it = str_iterator(s);

	isize count = 0;
	rune c; i8 len;
	while(utf8_iter_next(&it, &c, &len)){
		count += 1;
	}
	return count;
}

isize str_codepoint_offset(string s, isize n){
	struct utf8_iterator it = str_iterator(s);

	isize acc = 0;

	rune c; i8 len;
	do {
		if(acc == n){ break; }
		acc += 1;
	} while(utf8_iter_next(&it, &c, &len));

	return it.current;
}

// TODO: Handle length in codepoint count
string str_sub(string s, isize start, isize byte_count){
	if(start < 0 || byte_count < 0 || (start + byte_count) > s.len){ return EMPTY; }

	string sub = {
		.data = &s.data[start],
		.len = byte_count,
	};

	return sub;
}

bool str_eq(string a, string b){
	if(a.len != b.len){ return false; }

	for(isize i = 0; i < a.len; i += 1){
		if(a.data[i] != b.data[i]){ return false; }
	}

	return true;
}

#define MAX_CUTSET_LEN 64

string str_trim_leading(string s, string cutset){
	/* debug_assert(cutset.len <= MAX_CUTSET_LEN, "Cutset string exceeds MAX_CUTSET_LEN"); */

	rune set[MAX_CUTSET_LEN] = {0};
	isize set_len = 0;
	isize cut_after = 0;

decode_cutset: {
		rune c; i8 n;
		struct utf8_iterator iter = str_iterator(cutset);

		isize i = 0;
		while(utf8_iter_next(&iter, &c, &n) && i < MAX_CUTSET_LEN){
			set[i] = c;
			i += 1;
		}
		set_len = i;
	}

strip_cutset: {
		rune c; i8 n;
		struct utf8_iterator iter = str_iterator(s);

		while(utf8_iter_next(&iter, &c, &n)){
			bool to_be_cut = false;
			for(isize i = 0; i < set_len; i += 1){
				if(set[i] == c){
					to_be_cut = true;
					break;
				}
			}

			if(to_be_cut){
				cut_after += n;
			}
			else {
				break; // Reached first rune that isn't in cutset
			}

		}
	}

	return str_sub(s, cut_after, s.len - cut_after);
}

string str_trim_trailing(string s, string cutset){
	/* debug_assert(cutset.len <= MAX_CUTSET_LEN, "Cutset string exceeds MAX_CUTSET_LEN"); */

	rune set[MAX_CUTSET_LEN] = {0};
	isize set_len = 0;
	isize cut_until = s.len;

decode_cutset: {
		rune c; i8 n;
		struct utf8_iterator iter = str_iterator(cutset);

		isize i = 0;
		while(utf8_iter_next(&iter, &c, &n) && i < MAX_CUTSET_LEN){
			set[i] = c;
			i += 1;
		}
		set_len = i;
	}

strip_cutset: {
		rune c; i8 n;
		struct utf8_iterator iter = str_iterator_reversed(s);

		while(utf8_iter_prev(&iter, &c, &n)){
			bool to_be_cut = false;
			for(isize i = 0; i < set_len; i += 1){
				if(set[i] == c){
					to_be_cut = true;
					break;
				}
			}

			if(to_be_cut){
				cut_until -= n;
			}
			else {
				break; // Reached first rune that isn't in cutset
			}

		}
	}

	return str_sub(s, 0, cut_until);
}

string str_trim(string s, string cutset){
	string st = str_trim_leading(str_trim_trailing(s, cutset), cutset);
	return st;
}

#undef SURROGATE2
#undef SURROGATE1
#undef MASK2
#undef MASK3
#undef MASK4
#undef MASKX
#undef SIZE2
#undef SIZE3
#undef SIZE4
#undef CONT
#undef CONTINUATION1
#undef CONTINUATION2

