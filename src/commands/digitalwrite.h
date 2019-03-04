#ifndef INO_DIGITAL_WRITE_H
#define INO_DIGITAL_WRITE_H

#include "Command.h"

namespace ino {

int cmd_digitalwrite(Span<StringView<>> argv);

template <>
[[gnu::progmem]]
inline constexpr auto command_traits<cmd_digitalwrite> = CommandTraits{
	"digitalwrite",
	"digitalwrite <pin> <value>",
	"Drive the given pin HIGH (1) or LOW (0)."
};


} /* namespace ino */
#endif /* INO_DIGITAL_WRITE_H */
