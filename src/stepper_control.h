#ifndef STEPPER_CONTROL_H
#define STEPPER_CONTROL_H
#include "Command.h"

namespace ino {

int cmd_stepper(int argc, char** argv);

template <>
[[gnu::progmem]]
inline constexpr auto command_traits<cmd_stepper> = ino::CommandTraits{
	"stepper",
	"stepper [position]",
	"Get or set the stepper position with a value from 0 to 99."
};

} /* namespace ino */

#endif /* STEPPER_CONTROL_H */
