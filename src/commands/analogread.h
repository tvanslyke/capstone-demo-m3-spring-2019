#ifndef INO_ANALOG_READ_H
#define INO_ANALOG_READ_H

#include "Command.h"
#include "StringView.h"
#include "Span.h"

namespace ino {

int cmd_analogread(Span<StringView<>> argv);

template <>
[[gnu::progmem]]
inline constexpr auto command_traits<cmd_analogread> = CommandTraits{
	"analogread",
	"analogread <pin>",
	"Show the analog voltage reading in the range [0, 1024) for the pin."
};

} /* namespace ino */
#endif /* INO_ANALOG_READ_H */
