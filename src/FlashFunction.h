#ifndef INO_FLASH_FUNCTION_H
#define INO_FLASH_FUNCTION_H

#include "ProgmemPtr.h"

namespace ino {

template <class Sig>
struct FlashFunction;

template <class R, class ... Args>
struct FlashFunction<R(Args...)> {
	using function_type = R (*)(Args...);

	constexpr FlashFunction(function_type fn):
		func_(fn)
	{
		
	}

	constexpr R operator()(Args ... args) const {
		return load_from_flash(func_)(args...);
	}

	explicit constexpr operator bool() const {
		return func_;
	}

private:
	function_type func_;
};

template <class R, class ... Args>
FlashFunction(R (*)(Args ...)) -> FlashFunction<R(Args...)>;

template <class F>
struct flash_function_type{};

template <class R, class ... Args>
struct flash_function_type<R(*)(Args...)> {
	using type = ino::FlashFunction<R(Args...)>;
};

template <class F>
using flash_function_type_t = typename ino::flash_function_type<T>::type;

} /* namespace ino */

#endif /* INO_FLASH_FUNCTION_H */
