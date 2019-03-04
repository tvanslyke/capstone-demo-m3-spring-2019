#ifndef INO_PINMODE_H
#define INO_PINMODE_H

#include "Command.h"
namespace ino {

int cmd_pinmode(Span<StringView<>> argv);

template <>
[[gnu::progmem]]
inline constexpr auto command_traits<cmd_pinmode> = CommandTraits{
	"pinmode",
	"pinmode <pin> [mode]",
	"Get or set the mode for the given pin."
};

} /* namespace ino */

#endif /* INO_PINMODE_H */
