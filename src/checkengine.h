#ifndef INO_CHECKENGINE_H
#define INO_CHECKENGINE_H
#include "Command.h"
namespace ino {

void checkengine_interrupt();

int cmd_checkengine_status(int argc, char** argv);

int cmd_checkengine_light(int argc, char** argv);

template <>
[[gnu::progmem]]
inline constexpr auto command_traits<cmd_checkengine_status> = ino::CommandTraits{
	"checkengine_status",
	"checkengine_status",
	"Check whether there is a problem with the engine even when the check engine light is off."
};

template <>
[[gnu::progmem]]
inline constexpr auto command_traits<cmd_checkengine_light> = ino::CommandTraits{
	"checkengine_light",
	"checkengine_light [ON/OFF]",
	"Get or set whether the check engine light is on or off."
};



} /* namespace ino */

#endif /* INO_CHECKENGINE_H */
