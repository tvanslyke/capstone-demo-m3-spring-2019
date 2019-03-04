#ifndef INO_STRING_VIEW_H
#define INO_STRING_VIEW_H

#include <cstddef>
#include <cstring>

// Arduino macros that break the STL
#undef min
#undef max

#include <limits>
#include "Optional.h"

namespace ino {

// Helper class to allow StringView to work with Serial.print().
template <class Char>
struct PrintableStringView;

// Partial implementation of std::basic_string_view:
//   https://en.cppreference.com/w/cpp/string/basic_string_view
template <class Char = char>
struct StringView {
	using value_type 	     = Char;
	using pointer 	             = const Char*;
	using const_pointer 	     = pointer;
	using reference 	     = const Char&;
	using const_reference 	     = reference;
	using iterator 	             = pointer;
	using const_iterator 	     = const_pointer;
	using reverse_iterator 	     = std::reverse_iterator<iterator>;
	using const_reverse_iterator = std::reverse_iterator<const_iterator>;
	using size_type 	     = std::size_t;
	using difference_type 	     = std::ptrdiff_t;

	static constexpr std::size_t npos = ~size_type(0);

	constexpr StringView() = default;

	constexpr StringView(const Char* p, size_type sz):
		data_(p), size_(sz)
	{
		
	}

	template <size_type N>
	constexpr StringView(const Char (&data)[N]):
		data_(data), size_(N - 1)
	{
		static_assert(N > 0u, "");
	}

	explicit constexpr StringView(const Char *data):
		data_(data), size_(std::strlen(data))
	{
		
	}

	constexpr size_type size() const {
		return size_;
	}

	constexpr pointer data() const {
		return data_;
	}

	constexpr reference operator[](size_type index) const {
		return data()[index];
	}

	constexpr reference front() const {
		return (*this)[0u];
	}

	constexpr reference back() const {
		return (*this)[size() - 1u];
	}

	constexpr bool empty() const {
		return size() == 0u;
	}

	constexpr iterator  begin() const { return data(); }
	constexpr iterator cbegin() const { return begin(); }
	constexpr iterator    end() const { return data() + size(); }
	constexpr iterator   cend() const { return end(); }

	constexpr reverse_iterator  rbegin() const { return {end()}; }
	constexpr reverse_iterator crbegin() const { return rbegin(); }
	constexpr reverse_iterator    rend() const { return {begin()}; }
	constexpr reverse_iterator   crend() const { return rend(); }

	std::size_t print_to(Print& print) const {
		return print.write(reinterpret_cast<const uint8_t*>(data()), size());
	}

	constexpr void remove_suffix(size_type len) {
		size_ -= len;
	}

	constexpr void remove_prefix(size_type len) {
		remove_suffix(len);
		data_ += len;
	}

	constexpr StringView substr(size_type pos = 0, size_type count = npos) const {
		auto cpy = *this;
		if(pos > size()) {
			cpy.data_ += cpy.size();
			cpy.size_ = 0u;
			return cpy;
		}
		cpy.remove_prefix(pos);
		if(cpy.size() > count) {
			cpy.size_ = count;
		}
		return cpy;
	}

	friend constexpr bool operator==(const StringView& lhs, const char* rhs) {
		for(size_type i = 0u; i < lhs.size(); ++i) {
			char c1 = lhs[i];
			char c2 = *rhs;
			if(c1 != c2) {
				return false;
			}
			if(c2 == '\0') {
				if(i == lhs.size() - 1u) {
					return true;
				}
				return false;
			}
			++rhs;
		}
		return *rhs == '\0';
	}

	friend constexpr bool operator==(const char* lhs, const StringView& rhs) {
		return rhs == lhs;
	}

	friend constexpr bool operator!=(const StringView& lhs, const char* rhs) {
		return not (lhs == rhs);
	}

	friend constexpr bool operator!=(const char* lhs, const StringView& rhs) {
		return rhs != lhs;
	}

	constexpr operator PrintableStringView<Char>() const;
private:
	const Char* data_ = nullptr;
	std::size_t size_ = 0ul;
};

template <class Char>
StringView(const Char*, std::size_t) -> StringView<Char>;

template <class Char>
StringView(const Char*) -> StringView<Char>;

StringView() -> StringView<char>;

template <class Char>
struct PrintableStringView final: Printable {

	constexpr PrintableStringView(StringView<Char> s):
		str_(s)
	{
		
	}

	virtual std::size_t printTo(Print& p) const final override {
		return str_.print_to(p);
	}

private:
	StringView<Char> str_;
};

template <class Char>
constexpr StringView<Char>::operator PrintableStringView<Char>() const {
	return {*this};
}


template <class C>
bool operator==(FlashStringView<C> lhs, StringView<C> rhs) {
	if(lhs.size() != rhs.size()) {
		return false;
	}
	auto sz = lhs.size();
	for(std::size_t i = 0u; i < sz; ++i) {
		char c = lhs[i];
		if(c != rhs[i]) {
			return false;
		}
	}
	return true;
}

template <class C>
bool operator==(StringView<C> lhs, FlashStringView<C> rhs) {
	return rhs == lhs;
}

template <class C>
bool operator!=(FlashStringView<C> lhs, StringView<C> rhs) {
	return not (lhs == rhs);
}

template <class C>
bool operator!=(StringView<C> lhs, FlashStringView<C> rhs) {
	return rhs != lhs;
}


template <class Int>
constexpr Optional<Int> parse_decimal(StringView<> sv) {
	using limits = std::numeric_limits<Int>;
	Int value = 0;
	if(sv.empty()) {
		return nullopt;
	}
	bool negate = false;
	
	switch(sv.front()) {
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
		value = static_cast<Int>(sv.front() - '0');
		[[fallthrough]]
	case '+':
		break;
	case '-':
		if(not limits::is_signed) {
			// negative number but 'Int' is unsigned.
			return nullopt;
		}
		negate = true;
	}

	sv.remove_prefix(1);
	for(char c: sv) {
		if(c >= '0' and c <= '9') {
			if((limits::max() / 10) < value) {
				// multiplication would overflow.
				return nullopt;
			}
			value *= 10;
			Int digit = c - '0';
			if(limits::max() - value < digit) {
				// addition would overflow.
				return nullopt;
			}
			value += digit;
		} else {
			break;
		}
	}
	if(negate) {
		value = -value;
	}
	return {value};
}


} /* namespace ino */

#endif /* INO_STRING_VIEW_H */
