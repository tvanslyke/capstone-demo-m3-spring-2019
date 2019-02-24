#ifndef INO_FLASH_STRING_H
#define INO_FLASH_STRING_H

#include "ProgmemPtr.h"
#include <iterator>

namespace ino {

template <class Char = char>
struct PrintableFlashStringView;

template <std::size_t N>
struct FlashString;


template <class Char = char>
struct FlashStringView {
	using value_type 	     = Char;
	using pointer 	             = ProgmemPtr<Char>;
	using const_pointer 	     = pointer;
	using reference 	     = ProgmemRef<Char>;
	using const_reference 	     = reference;
	using const_iterator 	     = pointer;
	using iterator 	             = pointer;
	using reverse_iterator 	     = std::reverse_iterator<iterator>;
	using const_reverse_iterator = std::reverse_iterator<const_iterator>;
	using size_type 	     = std::size_t;
	using difference_type 	     = std::ptrdiff_t;

	static constexpr std::size_t npos = ~size_type(0);

	constexpr FlashStringView() = default;

	template <std::size_t N>
	constexpr FlashStringView(const FlashString<N>& s);

	constexpr FlashStringView(const Char* p, size_type sz):
		data_(p), size_(sz)
	{
		
	}

	template <size_type N>
	constexpr FlashStringView(const Char (&data)[N]):
		data_(data), size_(N - 1)
	{
		static_assert(N > 0u, "");
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
		std::size_t count = 0u;
		unsigned char buff[32];
		auto cpy = *this;
		while(cpy.size() >= sizeof(buff)) {
			memcpy_P(buff, cpy.data().flash_address(), sizeof(buff));
			count += print.write(buff, sizeof(buff));
			cpy.remove_prefix(sizeof(buff));
		}
		memcpy_P(buff, cpy.data().flash_address(), cpy.size());
		count += print.write(buff, cpy.size());
		return count;
	}

	constexpr void remove_suffix(size_type len) {
		size_ -= len;
	}

	constexpr void remove_prefix(size_type len) {
		remove_suffix(len);
		data_ += len;
	}

	constexpr FlashStringView substr(size_type pos = 0, size_type count = npos) const {
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

	friend constexpr bool operator==(const FlashStringView& lhs, const char* rhs) {
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

	friend constexpr bool operator==(const char* lhs, const FlashStringView& rhs) {
		return rhs == lhs;
	}

	friend constexpr bool operator!=(const FlashStringView& lhs, const char* rhs) {
		return not (lhs == rhs);
	}

	friend constexpr bool operator!=(const char* lhs, const FlashStringView& rhs) {
		return rhs != lhs;
	}

	constexpr operator PrintableFlashStringView<Char>() const;
private:
	ino::ProgmemPtr<Char> data_ = nullptr;
	std::size_t size_ = 0ul;
};

template <class Char>
struct PrintableFlashStringView final: Printable {

	constexpr PrintableFlashStringView(FlashStringView<Char> s):
		str_(s)
	{
		
	}

	virtual std::size_t printTo(Print& p) const final override {
		return str_.print_to(p);
	}

private:
	FlashStringView<Char> str_;
};

template <class Char>
constexpr FlashStringView<Char>::operator PrintableFlashStringView<Char>() const {
	return {*this};
}

namespace detail {

template <std::size_t ... I>
struct index_sequence { };

template <std::size_t N, std::size_t ... I>
struct make_index_sequence_type;

template <std::size_t ... I>
struct make_index_sequence_type<0u, I...> {
	using type = ino::detail::index_sequence<I ...>;
};

template <std::size_t N, std::size_t ... I>
struct make_index_sequence_type {
	using type = typename ino::detail::make_index_sequence_type<N - 1u, N - 1u, I ...>::type;
};

template <std::size_t N>
using make_index_sequence = typename ino::detail::make_index_sequence_type<N>::type;

} /* namespace detail */

template <std::size_t N>
struct FlashString {
	
	static constexpr std::size_t npos = ino::FlashStringView<char>::npos;

	using value_type 	     = char;
	using pointer 	             = ProgmemPtr<char>;
	using const_pointer 	     = pointer;
	using reference 	     = ProgmemRef<char>;
	using const_reference 	     = reference;
	using const_iterator 	     = pointer;
	using iterator 	             = pointer;
	using reverse_iterator 	     = std::reverse_iterator<iterator>;
	using const_reverse_iterator = std::reverse_iterator<const_iterator>;
	using size_type 	     = std::size_t;
	using difference_type 	     = std::ptrdiff_t;

private:

	template <std::size_t ... I>
	constexpr FlashString(const char (&str)[N + 1u], ino::detail::index_sequence<I...>) noexcept:
		data_{str[I] ..., '\0'}
	{
		
	}
public:

	constexpr FlashString(const char (&str)[N + 1u]) noexcept:
		FlashString(str, ino::detail::make_index_sequence<N>{})
	{
		
	}

	FlashString() = delete;
	FlashString(const FlashString&) = delete;
	FlashString(FlashString&&) = delete;

	FlashString& operator=(const FlashString&) = delete;
	FlashString& operator=(FlashString&&) = delete;

	constexpr size_type size() const noexcept { return N; }

	constexpr pointer data() const noexcept { return data_; }

	constexpr iterator                 begin() const noexcept { return data(); }
	constexpr const_iterator          cbegin() const noexcept { return begin(); }
	constexpr reverse_iterator        rbegin() const noexcept { return {end()}; }
	constexpr const_reverse_iterator crbegin() const noexcept { return {cend()}; }

	constexpr iterator                   end() const noexcept { return data() + N; }
	constexpr const_iterator            cend() const noexcept { return end(); }
	constexpr reverse_iterator          rend() const noexcept { return {begin()}; }
	constexpr const_reverse_iterator   crend() const noexcept { return {cbegin()}; }


	constexpr reference front() const {
		static_assert(N > 0u);
		return (*this)[0u];
	}

	constexpr reference back() const {
		static_assert(N > 0u);
		return (*this)[N - 1u];
	}

	constexpr reference operator[](size_type index) const {
		return view()[index];
	}

	constexpr FlashStringView<char> substr(size_type pos = 0, size_type count = npos) const {
		return view().substr(pos, count);
	}
	
	constexpr FlashStringView<char> view() const noexcept {
		return FlashStringView<char>(&data_[0], N);
	}

	constexpr operator FlashStringView<char>() const {
		return view();
	}

	friend constexpr bool operator==(const FlashString& lhs, const FlashString& rhs) {
		return lhs.view() == rhs.view();
	}

	friend constexpr bool operator==(const char* lhs, const FlashString& rhs) {
		return lhs == rhs.view();
	}

	friend constexpr bool operator==(const FlashString& lhs, const char* rhs) {
		return lhs.view() == rhs;
	}

	friend constexpr bool operator==(FlashStringView<char> lhs, const FlashString& rhs) {
		return lhs == rhs.view();
	}

	friend constexpr bool operator==(const FlashString& lhs, FlashStringView<char> rhs) {
		return lhs.view() == rhs;
	}

	friend constexpr bool operator!=(const FlashString& lhs, const FlashString& rhs) {
		return lhs.view() != rhs.view();
	}

	friend constexpr bool operator!=(const char* lhs, const FlashString& rhs) {
		return lhs != rhs.view();
	}

	friend constexpr bool operator!=(const FlashString& lhs, const char* rhs) {
		return lhs.view() != rhs;
	}

	friend constexpr bool operator!=(FlashStringView<char> lhs, const FlashString& rhs) {
		return lhs != rhs.view();
	}

	friend constexpr bool operator!=(const FlashString& lhs, FlashStringView<char> rhs) {
		return lhs.view() != rhs;
	}

	constexpr operator PrintableFlashStringView<char>() const {
		return {view()};
	}


private:
	const char data_[N + 1u];
};

template <std::size_t N>
FlashStringView(const FlashString<N>&) -> FlashStringView<char>;
 
template <std::size_t N>
FlashString(const char (&str)[N]) -> FlashString<N - 1u>;


[[gnu::progmem]]
inline constexpr auto empty_flash_string = ino::FlashString{""};

 
} /* namespace ino */

#endif /* INO_FLASH_STRING_H */
