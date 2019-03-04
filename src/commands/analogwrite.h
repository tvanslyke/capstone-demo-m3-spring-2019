#ifndef INO_ANALOG_WRITE_H
#define INO_ANALOG_WRITE_H

#include "Command.h"
#include "StringView.h"
#include "Span.h"

namespace ino {

int cmd_analogwrite(Span<StringView<>> argv);

template <>
[[gnu::progmem]]
inline constexpr auto command_traits<cmd_analogwrite> = CommandTraits{
	"analogwrite",
	"analogwrite <pin> <value>",
	"Drive the given PWM pin with a pulse width in the range [0, 256)."
};


} /* namespace ino */
#endif /* INO_ANALOG_WRITE_H */
