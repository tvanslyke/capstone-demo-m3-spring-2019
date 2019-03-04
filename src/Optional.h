#ifndef INO_OPTIONAL_H
#define INO_OPTIONAL_H


namespace ino {

inline constexpr struct nullopt_t {} nullopt;

template <class T>
struct Optional {
	constexpr Optional() = default;

	constexpr Optional(nullopt_t):
		Optional()
	{
	
	}

	template <class U>
	constexpr Optional(U&& value):
		value_(static_cast<U&&>(value)),
		has_value_(true)
	{
	
	}

	constexpr Optional(const Optional& other):
		Optional()
	{
		if(other.has_value()) {
			emplace_fast(other.value());
		}
	}

	constexpr Optional(Optional&& other):
		Optional()
	{
		if(other.has_value()) {
			emplace_fast(static_cast<T&&>(other.value()));
		}
	}


	~Optional() {
		reset();
	}

	constexpr operator bool () const {
		return has_value();
	}

	constexpr bool has_value() const {
		return has_value_;
	}

	void reset() {
		if(has_value()) {
			// manually destroy the contained value
			value_.~T();
			has_value_ = false;
		}
	}

	template <class ... Args>
	T& emplace(Args&& ... args) {
		reset();
		// Not an allocation! (placement new)
		return emplace_fast(static_cast<Args&&>(args)...);
	}

	constexpr const T& value() const {
		return *value_;
	}

	constexpr T& value() {
		return *value_;
	}

	constexpr const T& operator*() const {
		return value();
	}

	constexpr T& operator*() {
		return value();
	}

	constexpr const T* operator->() const {
		return &value();
	}

	constexpr T* operator->() {
		return &value();
	}

	template <class U>
	constexpr T value_or(U&& other) const& {
		if(*this) {
			return *this;
		} else {
			return static_cast<T>(static_cast<U&&>(other));
		}
	}

	template <class U>
	constexpr T value_or(U&& other) const&& {
		return static_cast<const Optional*>(this)->value_or(static_cast<U&&>(other));
	}

	template <class U>
	constexpr T value_or(U&& other) & {
		return static_cast<const Optional*>(this)->value_or(static_cast<U&&>(other));
	}

	template <class U>
	constexpr T value_or(U&& other) && {
		if(*this) {
			return static_cast<T&&>(*this);
		} else {
			return static_cast<T>(static_cast<U&&>(other));
		}
	}

private:

	template <class ... Args>
	constexpr T& emplace_fast(Args&& ... args) {
		// Not an allocation! (placement new)
		T& v = *(new (&value_) T(static_cast<Args&&>(args) ...));
		has_value_ = true;
		return v;
	}

	union {
		char dummy_ = '\0';
		T value_;
	};
	bool has_value_ = false;
};

} /* namespace ino */

#endif /* INO_OPTIONAL_H */
