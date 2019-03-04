#ifndef INO_SPAN_H
#define INO_SPAN_H

#include <cstddef>
#include <iterator>
#include "assert.h"
#include "Array.h"

namespace ino {

/**
 * Implementation of most of std::span<T>'s interface:
 *     https://en.cppreference.com/w/cpp/container/span
 * In short a fancy (pointer, length) pair.
 */
template <class T>
struct Span {
	using value_type             = T;
	using pointer                = T*;
	using const_pointer          = const T*;
	using reference              = T&;
	using const_reference        = const T&;
	using iterator               = pointer;
	using const_iterator         = const_pointer;
	using reverse_iterator       = std::reverse_iterator<pointer>;
	using const_reverse_iterator = std::reverse_iterator<const_pointer>;
	using size_type              = std::size_t;
	using difference_type        = std::ptrdiff_t;

	constexpr Span():
		data_(nullptr), size_(0u)
	{
		
	}

	constexpr Span(pointer d, size_type sz):
		data_(d), size_(sz)
	{
		
	}

	template <size_type N>
	constexpr Span(T (&array)[N]):
		Span(array, N)
	{
		
	}

	template <size_type N>
	constexpr Span(const ino::Array<T, N>& array):
		Span(array.data(), N)
	{
		
	}

	template <size_type N>
	constexpr Span(ino::Array<T, N>& array):
		Span(array.data(), N)
	{
		
	}

	constexpr               iterator   begin() const { return data(); }
	constexpr         const_iterator  cbegin() const { return data(); }
	constexpr       reverse_iterator  rbegin() const { return {end()}; }
	constexpr const_reverse_iterator crbegin() const { return {end()}; }

	constexpr               iterator   end() const { return data() + size(); }
	constexpr         const_iterator  cend() const { return data() + size(); }
	constexpr       reverse_iterator  rend() const { return {begin()}; }
	constexpr const_reverse_iterator crend() const { return {begin()}; }

	constexpr pointer    data() const { return data_; }
	constexpr size_type  size() const { return size_; }
	constexpr bool      empty() const { return size() == 0u; }

	constexpr reference operator[](size_type index) const {
		return data()[index];
	}

	constexpr reference front() const {
		return (*this)[0u];
	}

	constexpr reference back() const {
		return (*this)[size() - 1u];
	}

	constexpr Span subspan(size_type start, size_type count = -1) const {
		count = (count == static_cast<size_type>(-1)) ? size() - start : count;
		auto cpy = *this;
		cpy.data_ += start;
		cpy.size_ = count;
		return cpy;
	}

	constexpr Span first(size_type count) const {
		return subspan(0u, count);
	}
	
	constexpr Span last(size_type count) const {
		return subspan(size() - count, count);
	}
	
private:
	pointer data_;
	size_type size_;
};

template <class T, std::size_t N>
Span(T (&)[N]) -> Span<T>;

template <class T, std::size_t N>
Span(const T (&)[N]) -> Span<const T>;

template <class T>
Span(T*, std::size_t) -> Span<T>;

template <class T>
Span(const T*, std::size_t) -> Span<const T>;

template <class T, std::size_t N>
Span(ino::Array<T, N>&) -> Span<T>;

template <class T, std::size_t N>
Span(const ino::Array<T, N>&) -> Span<const T>;

template <class T, std::size_t N>
Span(ino::Array<const T, N>&) -> Span<const T>;

template <class T, std::size_t N>
Span(const ino::Array<const T, N>&) -> Span<const T>;

} /* namespace ino */

#endif /* INO_SPAN_H */
