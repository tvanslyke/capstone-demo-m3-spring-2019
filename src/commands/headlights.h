#ifndef INO_HEADLIGHTS_H
#define INO_HEADLIGHTS_H

#include "Command.h"

namespace ino {

int cmd_headlights(Span<StringView<>> argv);

template <>
[[gnu::progmem]]
inline constexpr auto command_traits<cmd_headlights> = CommandTraits{
	"headlights",
	"headlights [ON/OFF]",
	"Modify or query the state of the headlights."
};

} /* namespace ino */

#endif /* INO_HEADLIGHTS_H */
