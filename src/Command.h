#ifndef COMMAND_H
#define COMMAND_H

#include <cstdlib>
#include <cstring>
#include <Arduino.h>
#include "FlashString.h"

namespace ino {

enum class CommandTrait {
	Name,
	Usage,
	Description
};

struct CommandTraitsBase {};

template <std::size_t NameSz, std::size_t UsageSz, std::size_t DescrSz>
struct CommandTraits: CommandTraitsBase {
	using trait_getter_type = ino::FlashStringView<> (*)(const CommandTraitsBase&, CommandTrait);


	constexpr CommandTraits(
		const char (&name)[NameSz + 1],
		const char (&usage)[UsageSz + 1],
		const char (&descr)[DescrSz + 1]
	):
		CommandTraitsBase{},
		name_(name),
		usage_(usage),
		descr_(descr)
	{
		
	}

	static constexpr ino::FlashStringView<> get_trait(const CommandTraitsBase& traits, CommandTrait trait) {
		switch(trait) {
		case CommandTrait::Name:
			return static_cast<const CommandTraits*>(&traits)->name_;
		case CommandTrait::Usage:
			return static_cast<const CommandTraits*>(&traits)->usage_;
		case CommandTrait::Description:
			return static_cast<const CommandTraits*>(&traits)->descr_;
		}
	}
	
private:
	ino::FlashString<NameSz>  name_;
	ino::FlashString<UsageSz> usage_;
	ino::FlashString<DescrSz> descr_;
};

/* Deduction guide. */
template <std::size_t X, std::size_t Y, std::size_t Z>
CommandTraits(const char (&)[X], const char (&)[Y], const char (&)[Z]) -> CommandTraits<X-1, Y-1, Z-1>;

/* Specialize this for each command. */
template <int (*Cmd)(int, char**)>
inline constexpr auto command_traits = ino::CommandTraits{"Name", "Usage", "Description"};

template <class First, class ... Rest>
[[nodiscard]]
int command_error_helper(const First& first, const Rest& ... rest) {
	if constexpr(sizeof...(Rest) == 0u) {
		Serial.println(first);
		return -1;
	} else {
		return command_error_helper(rest...);
	}
}

template <class ... Args>
[[nodiscard]]
int command_error(const Args& ... args) {
	Serial.print(F("Error: "));
	(Serial.print(args) , ... , Serial.println());
	return -1;
}

int invoke_command(int argc, char** argv);

} /* namespace ino */

#endif /* COMMAND_H */
