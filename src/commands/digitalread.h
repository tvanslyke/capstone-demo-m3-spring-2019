#ifndef INO_DIGITAL_READ_H
#define INO_DIGITAL_READ_H

#include "Command.h"

namespace ino {

int cmd_digitalread(Span<StringView<>>);

template <>
[[gnu::progmem]]
inline constexpr auto command_traits<cmd_digitalread> = CommandTraits{
	"digitalread",
	"digitalread <pin>",
	"Read whether the given pin is HIGH (1) or LOW (0)."
};

} /* namespace ino */
#endif /* INO_DIGITAL_READ_H */
