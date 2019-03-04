#ifndef STEPPER_CONTROL_H
#define STEPPER_CONTROL_H
#include "Command.h"

namespace ino {

int cmd_window(Span<StringView<>>);

template <>
[[gnu::progmem]]
inline constexpr auto command_traits<cmd_window> = ino::CommandTraits{
	"window",
	"window [OPEN/CLOSE]",
	"Get or set the window position."
};

} /* namespace ino */

#endif /* STEPPER_CONTROL_H */
