#ifndef COMMAND_H
#define COMMAND_H

#include <cstdlib>
#include <cstring>
#include <Arduino.h>
#include "FlashString.h"
#include "StringView.h"
#include "Span.h"
#include "Pins.h"

namespace ino {

int invoke_command(Span<StringView<>> argv);

const CheckedPin* pin_from_name(StringView<> name);

int commandargs_check(Span<StringView<>> argv, int min_args, int max_args = -1); 

const CheckedPin* pincommand_check(Span<StringView<>> argv, int min_args, int max_args = -1);

/**
 * Enumeration for each 
 */
enum class CommandTrait {
	Name,
	Usage,
	Description
};

struct CommandTraitsBase {};


/**
 * @tparam NameSz  - Size of the command 'name' string (NOT including null terminator).
 * @tparam UsageSz - Size of the command 'usage' string (NOT including null terminator).
 * @tparam DescrSz - Size of the command 'description' (NOT including null terminator).
 * 
 * @brief Class template used to store command trait strings.  When adding a command, 
 *        an instance of this class must be instantiated for the command function using the
 *        'command_traits' template variable.
 * 
 * @note Instances of this class template should be stored in flash/program memory.  This
 *       can be done by marking declarations with 'PROGMEM' or using the attribute [[gnu::progmem]].
 * @note You should never actually have to explicitly specify the string sizes for this 
 *       class template; the sizes will be deduced using CTAD.
 */
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

/**
 * Specialize this for each command as follows:
 *         template <>
 *         [[gnu::progmem]]
 *         inline constexpr auto command_traits<my_command_name> = ino::CommandTraits{
 *                 "command_name",
 *                 "command_name <with> <usage> [arguments]",
 *                 "A brief description of your command."
 *         };
 */
template <int (*Cmd)(Span<StringView<>>)>
inline constexpr auto command_traits = ino::CommandTraits{"Name", "Usage", "Description"};


/** Use this function to print an error message when an error occurs in a command. */
template <class ... Args>
[[nodiscard]]
int command_error(const Args& ... args) {
	Serial.print(F("Error: "));
	(Serial.print(args) , ... , Serial.println());
	return -1;
}

/** Print arguments to Serial (with a newline) in order and return 0. */
template <class ... Args>
[[nodiscard]]
int command_success(const Args& ... args) {
	(Serial.print(args) , ... , Serial.println());
	return 0;
}

} /* namespace ino */

#endif /* COMMAND_H */
