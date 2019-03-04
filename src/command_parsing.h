#ifndef COMMAND_PARSING_H
#define COMMAND_PARSING_H

#include <Arduino.h>
#include <algorithm>
#include <iterator>
#include <cstddef>
#include "ino_assert.h"
#include "StringView.h"

namespace ino {

[[nodiscard]]
ino::StringView<> find_token(ino::StringView<> str) {
	constexpr auto whitespace = StringView(" \t");
	// skip whitespace
	const char* tok_begin = std::find_if(
		str.begin(),
		str.end(),
		[](char c) { return c != ' ' and c != '\t'; }
	);
	// no token found
	if(tok_begin == str.end()) {
		return {};
	}
	// find next whitespace separator
	auto tok_end = std::find_if(
		tok_begin + 1,
		str.end(),
		[](char c) { return c == ' ' or c == '\t'; }
	);
	// return the token as a StringView
	return {tok_begin, tok_end - tok_begin};
}

template <std::size_t N>
[[nodiscard]]
int tokenize_line(ino::StringView<> (&buff)[N], ino::StringView<> line) {
	if(line.empty()) {
		return 0u;
	}
	for(std::size_t i = 0u; i < N; ++i) {
		// all done, no more data in the line
		auto token = find_token(line);
		// empty token implies the rest of the line is all whitespace.
		if(token.empty()) {
			return static_cast<int>(i);
		}
		buff[i] = token;
		// Remove the token from the front of the line.
		line.remove_prefix(token.end() - line.begin());
		if(line.empty()) {
			return i + 1u;
		}
	}
	// Too many tokens.
	return -1;
}

template <std::size_t N>
[[nodiscard]]
signed long read_line(HardwareSerial& ser, char (&buff)[N]) {
	// Read a line from the serial object into the buffer.
	char* buffpos = buff;
	char* const endpos = buff + N;
	while(buffpos < endpos) {
		int read_val = ser.read();
		// Keep trying to read data from serial.
		while(read_val == -1) {
			// Manual devirtualization.
			read_val = ser.HardwareSerial::read();
		}
		char chr = static_cast<char>(read_val);
		ASSERT(static_cast<char>(chr) != '\0');
		// Encountered newline; all done.
		if(static_cast<char>(chr) == '\n') {
			// Write a null terminator and return.
			*buffpos = '\0';
			return buffpos - buff;
		} else {
			// Otherwise write the character and continue reading.
			*buffpos++ = static_cast<char>(chr);
		}
	}
	// Too much data for the buffer.
	return -1;
}

} /* namespace ino */


#endif /* COMMAND_PARSING_H */
