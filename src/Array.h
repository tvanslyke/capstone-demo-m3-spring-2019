#ifndef INO_ARRAY_H
#define INO_ARRAY_H

#include <cstddef>
#include <iterator>
#include "ProgmemPtr.h"

namespace ino {

template <class T, std::size_t N>
struct Array {

	using value_type             = T;
	using reference              = T&;
	using const_reference        = const T&;
	using pointer                = T*;
	using const_pointer          = const T*;
	using iterator               = pointer;
	using const_iterator         = const_pointer;
	using reverse_iterator       = std::reverse_iterator<iterator>;
	using const_reverse_iterator = std::reverse_iterator<const_iterator>;

	constexpr const_reference operator[](std::size_t idx) const {
		return private_data_[idx];
	}

	constexpr reference operator[](std::size_t idx) {
		return private_data_[idx];
	}

	static constexpr std::size_t size() {
		return N;
	}

	constexpr const_pointer data() const {
		return private_data_;
	}

	constexpr pointer data() {
		return private_data_;
	}

	constexpr iterator                 begin()       { return data(); }
	constexpr const_iterator           begin() const { return data(); }
	constexpr const_iterator          cbegin() const { return data(); }
	constexpr reverse_iterator        rbegin()       { return {end()}; }
	constexpr const_reverse_iterator  rbegin() const { return {end()}; }
	constexpr const_reverse_iterator crbegin() const { return {end()}; }

	constexpr iterator                 end()       { return data() + size(); }
	constexpr const_iterator           end() const { return data() + size(); }
	constexpr const_iterator          cend() const { return data() + size(); }
	constexpr reverse_iterator        rend()       { return {begin()}; }
	constexpr const_reverse_iterator  rend() const { return {begin()}; }
	constexpr const_reverse_iterator crend() const { return {begin()}; }

	T private_data_[N];
};

template <class T, class ... U>
Array(const T&, const U& ...) -> Array<T, (sizeof...(U) + 1u)>;

template <class T, std::size_t N>
struct FlashArray {
	
	using value_type             = T;
	using reference              = ino::ProgmemRef<T>;
	using const_reference        = reference;
	using pointer                = ino::ProgmemPtr<T>;
	using const_pointer          = pointer;
	using iterator               = pointer;
	using const_iterator         = const_pointer;
	using reverse_iterator       = std::reverse_iterator<iterator>;
	using const_reverse_iterator = std::reverse_iterator<const_iterator>;

	constexpr const_reference operator[](std::size_t idx) const {
		return {this->private_data_[idx]};
	}

	static constexpr std::size_t size() {
		return N;
	}

	constexpr const_pointer data() const {
		return {private_data_};
	}


	constexpr iterator                 begin() const { return data(); }
	constexpr const_iterator          cbegin() const { return data(); }
	constexpr reverse_iterator        rbegin() const { return {end()}; }
	constexpr const_reverse_iterator crbegin() const { return {end()}; }

	constexpr const_iterator           end() const { return data() + size(); }
	constexpr const_iterator          cend() const { return data() + size(); }
	constexpr const_reverse_iterator  rend() const { return {begin()}; }
	constexpr const_reverse_iterator crend() const { return {begin()}; }

	T private_data_[N];
};

template <class T, class ... U>
Array(const T&, const U& ...) -> Array<T, (sizeof...(U) + 1u)>;

template <class T, class ... U>
FlashArray(const T&, const U& ...) -> FlashArray<T, (sizeof...(U) + 1u)>;


} /* namespace ino */

#endif /* INO_ARRAY_H */
