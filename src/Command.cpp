#include "Command.h"
#include "Pins.h"
#include "ino_assert.h"
#include "FlashString.h"
#include "Array.h"
#include <Arduino.h>
#include <cstring>
#include <algorithm>

// Command headers
#include "commands/pinmode.h"
#include "commands/digitalread.h"
#include "commands/digitalwrite.h"
#include "commands/analogread.h"
#include "commands/analogwrite.h"
#include "commands/headlights.h"
#include "commands/checkengine.h"
#include "commands/stepper_control.h"


namespace ino {

const CheckedPin* pin_from_name(StringView<> name) {
	auto pos = std::find_if(
		ino::all_pins.begin(),
		ino::all_pins.end(),
		[=](CheckedPin p) { return name == p.name(); }
	);
	if(pos == ino::all_pins.end()) {
		return nullptr;
	} else {
		return &(*pos);
	}
}

int commandargs_check(Span<StringView<>> argv, int min_args, int max_args) {
	if(argv.size() < min_args) {
		return command_error(
			F("Command '"),
			argv[0],
			F("' expects at least "),
			min_args - 1,
			F(" arguments.")
		);
	}
	max_args = max_args == -1 ? min_args : max_args;
	if(argv.size() > max_args) {
		return command_error(
			F("Command '"),
			argv[0],
			F("' expects at most "),
			max_args - 1,
			F(" arguments.")
		);
	}
	return 0;
}

const CheckedPin* pincommand_check(Span<StringView<>> argv, int min_args, int max_args) {
	int err = commandargs_check(argv, min_args, max_args);
	if(err != 0) {
		return nullptr;
	}
	auto* pin = pin_from_name(argv[1]);
	if(not pin) {
		int err = command_error(F("Invalid pin name '"), argv[1], F("'."));
		(void)err;
		return nullptr;
	}
	return pin;
}

struct Command {
	using command_type = int (*)(Span<StringView<>>);
	using trait_getter_type = ino::FlashStringView<> (*)(const CommandTraitsBase&, CommandTrait);

	Command() = default;

	constexpr ino::FlashStringView<> name() const {
		return trait_getter_(*traits_, CommandTrait::Name);
	}

	constexpr ino::FlashStringView<> usage() const {
		return trait_getter_(*traits_, CommandTrait::Usage);
	}

	constexpr ino::FlashStringView<> description() const {
		return trait_getter_(*traits_, CommandTrait::Description);
	}

	[[nodiscard]]
	constexpr int operator()(Span<StringView<>> argv) const {
		ASSERT(*this);
		return command_(argv);
	}

	template <int (*Cmd)(Span<StringView<>>)>
	friend constexpr Command make_command();

	constexpr operator bool() const {
		return command_ and traits_ and trait_getter_;
	}

private:
	constexpr Command(
		int (*cmd)(Span<StringView<>>),
		const CommandTraitsBase* traits,
		trait_getter_type getter
	):
		command_(cmd),
		traits_(traits),
		trait_getter_(getter)
	{
		
	}

	int (*command_)(Span<StringView<>>);
	const CommandTraitsBase* traits_ = nullptr;
	trait_getter_type trait_getter_ = nullptr;
};

template <int (*Cmd)(Span<StringView<>>)>
constexpr Command make_command() {
	return Command(
		Cmd,
		&command_traits<Cmd>,
		command_traits<Cmd>.get_trait
	);
}

template <int (*Cmd)(Span<StringView<>>)>
inline constexpr Command command = make_command<Cmd>();

static int cmd_help(Span<StringView<>>);

template <>
[[gnu::progmem]]
inline constexpr auto command_traits<cmd_help> = CommandTraits{
	"help",
	"help",
	"Print this help menu."
};

[[gnu::progmem]]
static constexpr auto command_table = ino::FlashArray{
	command<cmd_help>,
	command<cmd_pinmode>,
	command<cmd_digitalread>,
	command<cmd_digitalwrite>,
	command<cmd_analogread>,
	command<cmd_analogwrite>,
	command<cmd_window>,
	command<cmd_headlights>,
	command<cmd_checkengine_status>,
	command<cmd_checkengine_light>
};

static void print_left_justified(ino::FlashStringView<> s, std::size_t width) {
	std::size_t i = 0u;
	Serial.print(s);
	for(std::size_t i = s.size(); i < width; ++i) {
		Serial.print(' ');
	}
}

static void print_left_justified(const char* s, std::size_t width) {
	Serial.print(s);
	for(std::size_t i = std::strlen(s); i < width; ++i) {
		Serial.print(' ');
	}
}

template <class First, class Second, class ... Rest>
static constexpr decltype(auto) max(const First& f, const Second& s, const Rest& ... rest) {
	if constexpr(sizeof...(rest) == 0u) {
		return (f < s) ? s : f;
	} else {
		return ino::max(ino::max(f, s), rest ...);
	}
}

template <class Iterator, class Acc, class T>
constexpr T accumulate(Iterator first, Iterator last, Acc acc, T init) {
	for(auto pos = first; pos != last; ++pos) {
		init = acc(static_cast<T&&>(init), *pos);
	}
	return init;
}

static int cmd_help(Span<StringView<>>) {
	// Get the maximum width of the command name, usage and description strings respectively.
	struct Sizes {
		std::size_t name_sz;
		std::size_t usage_sz;
		std::size_t descr_sz;
	};
	constexpr auto sizes = ino::accumulate(
		command_table.begin(),
		command_table.end(),
		[](Sizes sizes, const auto& cmd) {
			return Sizes{
				ino::max(cmd.flash_address()->name().size(),        sizes.name_sz),
				ino::max(cmd.flash_address()->usage().size(),       sizes.usage_sz),
				ino::max(cmd.flash_address()->description().size(), sizes.descr_sz)
			};
		},
		Sizes{sizeof("Name"), sizeof("Usage"), sizeof("Description")}
	);
	constexpr auto line_sz = sizes.name_sz + sizes.usage_sz + sizes.descr_sz;
	// Print the header.
	ino::print_left_justified("Name",        sizes.name_sz + 1u);
	ino::print_left_justified("Usage",       sizes.usage_sz + 1u);
	ino::print_left_justified("Description", sizes.descr_sz + 1u);
	Serial.println("");
	for(std::size_t i = 0u; i < line_sz; ++i) {
		Serial.print('-');
	}
	Serial.println("");
	// Print each command.
	for(Command command: command_table) {
		ino::print_left_justified(command.name(),        sizes.name_sz + 1u);
		ino::print_left_justified(command.usage(),       sizes.usage_sz + 1u);
		ino::print_left_justified(command.description(), sizes.descr_sz + 1u);
		Serial.println("");
	};
	return 0;
}

int invoke_command(Span<StringView<>> argv) {
	if(argv.size() == 0) {
		// blank line
		return 0;
	}
	auto pos = std::find_if(
		command_table.begin(),
		command_table.end(),
		[argv](Command cmd) { return cmd.name() == argv[0]; }
	);
	if(pos == command_table.end()) {
		return command_error("Unknown command '", argv[0], "'.");
	} else {
		Command command = *pos;
		return command(argv);
	}
}

} /* namespace ino */


