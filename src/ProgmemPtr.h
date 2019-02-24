#ifndef INO_PROGMEM_PTR_H
#define INO_PROGMEM_PTR_H

#include <Arduino.h>
#include <avr/pgmspace.h>
#include <cstddef>
#include <stdint.h>
#include <iterator>


namespace ino {

template <class T>
struct ProgmemRef;

template <class T>
ProgmemRef(const T&) -> ProgmemRef<T>;

template <class T>
struct ProgmemPtr;

template <class T>
ProgmemRef(const T*) -> ProgmemRef<T>;

namespace detail {

template <class T>
struct remove_ref {
	using type = T;
};

template <class T>
struct remove_ref<T&> {
	using type = T;
};

template <class T>
struct remove_ref<T&&> {
	using type = T;
};

template <class T>
struct remove_const {
	using type = T;
};

template <class T>
struct remove_const<const T> {
	using type = T;
};

} /* namespace detail */

template <class T>
struct FlashTraits;

template <>
struct FlashTraits<char> {
	static char load(const char& value) {
		unsigned char c = pgm_read_byte(&value);
		return reinterpret_cast<const char&>(c);
	}
};

template <>
struct FlashTraits<unsigned char> {
	static unsigned char load(const unsigned char& value) {
		return pgm_read_byte(&value);
	}
};

template <>
struct FlashTraits<signed char> {
	static signed char load(const signed char& value) {
		unsigned char c = pgm_read_byte(&value);
		return reinterpret_cast<const signed char&>(c);
	}
};

template <>
struct FlashTraits<uint16_t> {
	static uint16_t load(const uint16_t& value) {
		return pgm_read_word(&value);
	}
};

template <>
struct FlashTraits<int16_t> {
	static int16_t load(const int16_t& value) {
		uint16_t tmp = pgm_read_word(&value);
		return reinterpret_cast<const int16_t&>(tmp);
	}
};

template <>
struct FlashTraits<uint32_t> {
	static uint32_t load(const uint32_t& value) {
		return pgm_read_dword(&value);
	}
};

template <>
struct FlashTraits<int32_t> {
	static int32_t load(const int32_t& value) {
		uint32_t tmp = pgm_read_dword(&value);
		return reinterpret_cast<const int32_t&>(tmp);
	}
};

template <class T>
struct FlashTraits<T*> {
	using type = T*;
	static type load(const type& value) {
		const void* tmp = pgm_read_ptr(&value);
		const T* p = static_cast<const T*>(tmp);
		return const_cast<T*>(p);
	}
};

template <class T>
struct FlashTraits<const T*> {
	using type = const T*;
	static type load(const type& value) {
		const void* tmp = pgm_read_ptr(&value);
		return static_cast<const T*>(tmp);
	}
};

template <>
struct FlashTraits<float> {
	static float load(const float& value) {
		return pgm_read_float(&value);
	}
};

template <>
struct FlashTraits<double> {
	static_assert(sizeof(double) == sizeof(float));
	static double load(const double& value) {
		return pgm_read_float(&value);
	}
};

template <class T>
struct FlashTraits {
	static T load(const T& val) {
		T value;
		if constexpr(sizeof(T) == 1u) {
			auto tmp = pgm_read_byte(&val);
			std::memcpy(&value, &tmp, sizeof(value));
			return value;
		} else if constexpr(sizeof(T) == sizeof(uint16_t)) {
			auto tmp = pgm_read_word(&val);
			std::memcpy(&value, &tmp, sizeof(value));
			return value;
		} else if constexpr(sizeof(T) == sizeof(uint32_t)) {
			auto tmp = pgm_read_dword(&val);
			std::memcpy(&value, &tmp, sizeof(value));
			return value;
		} else {
			memcpy_P(&value, &val, sizeof(value));
			return value;
		}
	}
};

template <class T>
struct ProgmemRef {

	using flash_traits = ino::FlashTraits<T>;
	static constexpr std::size_t object_size = sizeof(T);

	ProgmemRef() = delete;
	constexpr ProgmemRef(const ProgmemRef&) = default;
	constexpr ProgmemRef(ProgmemRef&&) = default;

	ProgmemRef& operator=(const ProgmemRef&) = delete;
	ProgmemRef& operator=(ProgmemRef&&) = delete;

	explicit constexpr ProgmemRef(const T& obj):
		addr_(&obj)
	{
		
	}

	T load() const {
		return flash_traits::load(*addr_);
	}

	operator T() const {
		return this->load();
	}

	constexpr const T* flash_address() const {
		return addr_;
	}

	constexpr const T& get() const {
		return *flash_address();
	}
	template <class MemberPtr>
	constexpr auto get_member(MemberPtr member_ptr) const {
		const auto* mem = &(addr_->*member_ptr);
		using type = typename detail::remove_const<typename detail::remove_ref<decltype(*mem)>::type>::type;
		return ProgmemRef<type>(*mem);
	}
	
private:
	const T* addr_;
};

template <class T>
struct ProgmemPtr {
	using value_type        = T;
	using reference         = ProgmemRef<T>;
	using pointer           = ProgmemPtr;
	using difference_type   = std::ptrdiff_t;
	using iterator_category = std::random_access_iterator_tag;

	ProgmemPtr() = default;

	constexpr ProgmemPtr(const T* p):
		addr_(p)
	{
		
	}
	
	template <class MemberPtr>
	constexpr auto operator->*(MemberPtr member_ptr) const {
		return (**this).get_member(member_ptr);
	}
	
	constexpr const T* flash_address() const {
		return addr_;
	}

	constexpr const T* get() const {
		return flash_address();
	}

	constexpr ProgmemRef<T> operator*() const {
		return ProgmemRef<T>(*addr_);
	}

	constexpr ProgmemPtr& operator++() {
		++addr_;
		return *this;
	}

	constexpr ProgmemPtr operator++(int) {
		auto cpy = *this;
		++(*this);
		return cpy;
	}

	constexpr ProgmemPtr& operator--() {
		--addr_;
		return *this;
	}

	constexpr ProgmemPtr operator--(int) {
		auto cpy = *this;
		--(*this);
		return cpy;
	}

	constexpr ProgmemPtr& operator+=(difference_type delta) {
		addr_ += delta;
		return *this;
	}

	constexpr ProgmemPtr& operator-=(difference_type delta) {
		return *this += -delta;
	}

	friend constexpr ProgmemPtr operator+(ProgmemPtr lhs, difference_type rhs) {
		lhs += rhs;
		return lhs;
	}

	friend constexpr ProgmemPtr operator+(difference_type lhs, ProgmemPtr rhs) {
		rhs += lhs;
		return rhs;
	}

	friend constexpr ProgmemPtr operator-(ProgmemPtr lhs, difference_type rhs) {
		lhs -= rhs;
		return lhs;
	}

	friend constexpr difference_type operator-(ProgmemPtr lhs, ProgmemPtr rhs) {
		return lhs.addr_ - rhs.addr_;
	}

	constexpr reference operator[](std::ptrdiff_t delta) const {
		return *(*this + delta);
	}

	friend constexpr bool operator<(ProgmemPtr lhs, ProgmemPtr rhs) {
		return lhs.addr_ < rhs.addr_;
	}

	friend constexpr bool operator>(ProgmemPtr lhs, ProgmemPtr rhs) {
		return rhs.addr_ < lhs.addr_;
	}

	friend constexpr bool operator<=(ProgmemPtr lhs, ProgmemPtr rhs) {
		return lhs.addr_ <= rhs.addr_;
	}

	friend constexpr bool operator>=(ProgmemPtr lhs, ProgmemPtr rhs) {
		return lhs.addr_ >= rhs.addr_;
	}

	friend constexpr bool operator==(ProgmemPtr lhs, ProgmemPtr rhs) {
		return lhs.addr_ == rhs.addr_;
	}

	friend constexpr bool operator!=(ProgmemPtr lhs, ProgmemPtr rhs) {
		return lhs.addr_ != rhs.addr_;
	}

private:
	const T* addr_ = nullptr;
};

template <class T>
constexpr ProgmemPtr<T> operator&(ProgmemRef<T> ref) {
	return ProgmemPtr<T>(ref.flash_address());
}

template <class T>
T load_from_flash(const T& value) {
	return ino::ProgmemRef{value}.load();
}

} /* namespace ino */

#endif /* INO_PROGMEM_PTR_H */
