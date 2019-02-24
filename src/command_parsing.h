#ifndef COMMAND_PARSING_H
#define COMMAND_PARSING_H

#include <Arduino.h>
#include <algorithm>
#include <iterator>
#include <cstddef>
#include "ino_assert.h"

namespace ino {

[[nodiscard]]
std::pair<char*, std::size_t> find_token(char* first, char* last) {
	// skip whitespace
	auto tok_begin = std::find_if(
		first,
		last,
		[](char c){
			ASSERT(c != '\n');
			ASSERT(c != '\0');
			return c != ' ' or c != '\t';
		}
	);
	// no token found
	if(tok_begin == last) {
		return {nullptr, 0u};
	}
	auto tok_end = std::find_if(
		tok_begin + 1,
		last,
		[](char c){
			ASSERT(c != '\n');
			ASSERT(c != '\0');
			return c == ' ' or c == '\t';
		}
	);
	return {tok_begin, tok_end - tok_begin};
}

template <std::size_t N>
[[nodiscard]]
int tokenize_line(char* (&buff)[N], char* line, std::size_t line_size) {
	ASSERT(line[line_size] == '\0');
	for(std::size_t i = 0u; i < N; ++i) {
		if(line_size == 0u) {
			return i;
		}
		auto [pos, len] = find_token(line, line + line_size);
		if(not pos) {
			return static_cast<int>(i);
		}
		buff[i] = pos;
		line += len;
		line_size -= len;
		if(line_size == 0u) {
			return i + 1u;
		}
		*line++ = '\0';
		--line_size;
	}
	// Too many tokens.
	return -1;
}

template <std::size_t N>
[[nodiscard]]
signed long read_line(HardwareSerial& ser, char (&buff)[N]) {
	char* buffpos = buff;
	char* const endpos = buff + N;
	while(buffpos < endpos) {
		int chr = ser.read();
		while(chr == -1) {
			// Manual devirtualization.
			chr = ser.HardwareSerial::read();
		}
		ASSERT(static_cast<char>(chr) != '\0');
		if(static_cast<char>(chr) == '\n') {
			*buffpos = '\0';
			return buffpos - buff;
		}
		if(static_cast<char>(chr) == '\b') {
			if(buffpos > buff) {
				--buffpos;
			}
			continue;
		}
		*buffpos++ = static_cast<char>(chr);
	}
	return -1;
}

} /* namespace ino */


#endif /* COMMAND_PARSING_H */
