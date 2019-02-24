#include "Command.h"
#include "Pins.h"
#include "ino_assert.h"
#include "FlashString.h"
#include "Array.h"
#include <Arduino.h>
#include <cstring>
#include <algorithm>
#include "stepper_control.h"

namespace ino {
struct Command {
	using command_type = int (*)(int, char**);
	using trait_getter_type = ino::FlashStringView<> (*)(const CommandTraitsBase&, CommandTrait);

	constexpr Command() = default;

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
	constexpr int operator()(int argc, char** argv) const {
		ASSERT(*this);
		return command_(argc, argv);
	}

	template <int (*Cmd)(int, char**)>
	friend constexpr Command make_command();

	constexpr operator bool() const {
		return command_ and traits_ and trait_getter_;
	}

private:
	constexpr Command(
		int (*cmd)(int, char**),
		const CommandTraitsBase* traits,
		trait_getter_type getter
	):
		command_(cmd),
		traits_(traits),
		trait_getter_(getter)
	{
		
	}

	int (*command_)(int, char**) = nullptr;
	const CommandTraitsBase* traits_ = nullptr;
	trait_getter_type trait_getter_ = nullptr;
};

template <int (*Cmd)(int, char**)>
constexpr Command make_command() {
	return Command(
		Cmd,
		&command_traits<Cmd>,
		command_traits<Cmd>.get_trait
	);
}

template <int (*Cmd)(int, char**)>
inline constexpr Command command = make_command<Cmd>();


static const CheckedPin* pin_from_name(const char* name) {
	auto pos = std::find_if(
		&ino::all_pins[0],
		&ino::all_pins[0] + ino::pin_count,
		[=](CheckedPin p) { return std::strcmp(p.name(), name) == 0; }
	);
	if(pos == &ino::all_pins[0] + ino::pin_count) {
		return nullptr;
	} else {
		return &(*pos);
	}
}

static const int commandargs_check(int argc, char** argv, int min_args, int max_args = -1) {
	if(argc < min_args) {
		return command_error(
			F("Command '"),
			argv[0],
			F("' expects at least "),
			min_args - 1,
			F(" arguments.")
		);
	}
	max_args = max_args == -1 ? min_args : max_args;
	if(argc > max_args) {
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

static const CheckedPin* pincommand_check(int argc, char** argv, int min_args, int max_args = -1) {
	int err = commandargs_check(argc, argv, min_args, max_args);
	if(err != 0) {
		return nullptr;
	}
	auto* pin = pin_from_name(argv[1]);
	if(not pin) {
		int err = command_error(F("Invalid pin name '"), argv[1], "'.");
		(void)err;
		return nullptr;
	}
	return pin;
}

static int cmd_pinmode(int argc, char** argv) {
	auto* pin = pincommand_check(argc, argv, 2, 3);
	if(not pin) {
		return -1;
	}
	if(argc == 2) {
		switch(pin->mode()) {
		case PinMode::Input:
			Serial.println("INPUT");
			break;
		case PinMode::InputPullup:
			Serial.println("INPUT_PULLUP");
			break;
		case PinMode::Output:
			Serial.println("OUTPUT");
			break;
		}
		return 0;
	}
	if(std::strcmp(argv[2], "INPUT") == 0 or std::strcmp(argv[2], "input") == 0) {
		pin->set_mode(PinMode::Input);
		return 0;
	}
	if(std::strcmp(argv[2], "OUTPUT") == 0 or std::strcmp(argv[2], "output") == 0) {
		pin->set_mode(PinMode::Output);
		return 0;
	}
	if(std::strcmp(argv[2], "INPUT_PULLUP") == 0 or std::strcmp(argv[2], "input_pullup") == 0) {
		pin->set_mode(PinMode::InputPullup);
		return 0;
	}
	return command_error(F("Invalid pin mode '"), argv[2], "'.");
}

static int cmd_digitalread(int argc, char** argv) {
	auto* pin = pincommand_check(argc, argv, 2);
	if(not pin) {
		return -1;
	}
	switch(pin->digital_read()) {
	case LogicLevel::Low:
		Serial.println("0");
		break;
	case LogicLevel::High:
		Serial.println("1");
		break;
	}
	return 0;
}

static int cmd_digitalwrite(int argc, char** argv) {
	const auto* pin = pincommand_check(argc, argv, 3);
	if(not pin) {
		return -1;
	}
	LogicLevel logic_level = LogicLevel::Low;
	if(std::strcmp(argv[2], "low") == 0 or std::strcmp(argv[2], "LOW") == 0 or std::strcmp(argv[2], "0") == 0) {
		logic_level = LogicLevel::Low;
	} else if(std::strcmp(argv[2], "high") == 0 or std::strcmp(argv[2], "HIGH") == 0 or std::strcmp(argv[2], "1") == 0) {
		logic_level = LogicLevel::High;
	} else {
		return command_error(F("Invalid logic level '"), argv[2], "'.");
	}
	auto err = pin->digital_write(logic_level);
	if(err != PinStatus::Good) {
		return command_error("Pin ", argv[1], F(" is not currently in OUTPUT mode."));
	}
	return 0;
}

static int cmd_analogread(int argc, char** argv) {
	const auto* pin = pincommand_check(argc, argv, 2);
	if(not pin) {
		return -1;
	}
	auto result = pin->analog_read();
	int val = result.first;
	PinStatus status = result.second;
	if(status == PinStatus::BadPinKind) {
		return command_error("Pin ", argv[1], F(" is not an analog pin."));
	} else if(status == PinStatus::BadPinMode) {
		return command_error("Pin ", argv[1], F(" is not in INPUT or INPUT_PULLUP mode."));
	} else if(status != PinStatus::Good) {
		return command_error(F("Unable to read from pin "), argv[1], ".");
	}
	Serial.println(val);
	return 0;
}

static int cmd_ledon(int argc, char** argv) {
	if(argc != 1) {
		return command_error(F("Command 'ledon' takes no arguments."));
	}
	pin<13>.set_mode(PinMode::Output);
	if(pin<13>.digital_write(LogicLevel::High) != PinStatus::Good) {
		return command_error(F("Error while driving LED pin high."));
	}
	return 0;
}

static int cmd_ledoff(int argc, char** argv) {
	if(argc != 1) {
		return command_error(F("Command 'ledoff' takes no arguments."));
	}
	pin<13>.set_mode(PinMode::Output);
	if(pin<13>.digital_write(LogicLevel::Low) != PinStatus::Good) {
		return command_error(F("Error while driving LED pin high."));
	}
	return 0;
}


static int cmd_analogwrite(int argc, char** argv) {
	const auto* pin = pincommand_check(argc, argv, 3);
	if(not pin) {
		return -1;
	}
	long value = std::strtol(argv[2], nullptr, 10);
	if(value == 0 and argv[2][0] != '0') {
		return command_error("Cannot parse '", argv[2], F("' as a decimal integer in analogwrite."));
	}
	PinStatus status = pin->analog_write(value);
	switch(status) {
	default:
		return command_error("Unable to write to pin ", argv[1], ".");
		break;
	case PinStatus::BadAnalogWriteValue:
		return command_error(
			argv[2],
			F(" is out-of-range for analogwrite (must be in the range ["),
			CheckedPin::analog_write_minm,
			", ",
			CheckedPin::analog_write_maxm + 1,
			") )."
		);
		break;
	case PinStatus::BadPinKind:
		return command_error("Pin ", argv[1], " is not PWM-enabled.");
		break;
	case PinStatus::BadPinMode:
		return command_error("Pin ", argv[1], " is not in OUTPUT mode.");
		break;
	case PinStatus::Good:
		break;
	}
	return 0;
}

static int cmd_help(int argc, char** argv);

template <>
[[gnu::progmem]]
inline constexpr auto command_traits<cmd_help> = CommandTraits{
	"help",
	"help",
	"Print this help menu."
};

template <>
[[gnu::progmem]]
inline constexpr auto command_traits<cmd_pinmode> = CommandTraits{
	"pinmode",
	"pinmode <pin> [mode]",
	"Get or set the mode for the given pin."
};

template <>
[[gnu::progmem]]
inline constexpr auto command_traits<cmd_digitalread> = CommandTraits{
	"digitalread",
	"digitalread <pin>",
	"Read whether the given pin is HIGH (1) or LOW (0)."
};

template <>
[[gnu::progmem]]
inline constexpr auto command_traits<cmd_digitalwrite> = CommandTraits{
	"digitalwrite",
	"digitalwrite <pin> <value>",
	"Drive the given pin HIGH (1) or LOW (0)."
};

template <>
[[gnu::progmem]]
inline constexpr auto command_traits<cmd_analogread> = CommandTraits{
	"analogread",
	"analogread <pin>",
	"Show the analog voltage reading in the range [0, 1024) for the pin."
};

template <>
[[gnu::progmem]]
inline constexpr auto command_traits<cmd_analogwrite> = CommandTraits{
	"analogwrite",
	"analogwrite <pin> <value>",
	"Drive the given PWM pin with a pulse width in the range [0, 256)."
};

template <>
[[gnu::progmem]]
inline constexpr auto command_traits<cmd_ledon> = CommandTraits{
	"ledon",
	"ledon",
	"Turn the pin 13 LED on."
};

template <>
[[gnu::progmem]]
inline constexpr auto command_traits<cmd_ledoff> = CommandTraits{
	"ledoff",
	"ledoff",
	"Turn the pin 13 LED off."
};

[[gnu::progmem]]
static constexpr auto command_table = ino::FlashArray{
	command<cmd_help>,
	command<cmd_pinmode>,
	command<cmd_digitalread>,
	command<cmd_digitalwrite>,
	command<cmd_analogread>,
	command<cmd_analogwrite>,
	command<cmd_stepper>
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

static int cmd_help(int argc, char** argv) {

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

int invoke_command(int argc, char** argv) {
	if(argc == 0) {
		return 0;
	}
	ASSERT(argv[0]);
	auto pos = std::find_if(
		command_table.begin(),
		command_table.end(),
		[argv](Command cmd) { return cmd.name() == argv[0]; }
	);
	if(pos == command_table.end()) {
		return command_error("Unknown command '", argv[0], "'.");
	} else {
		Command command = *pos;
		return command(argc, argv);
	}
}

} /* namespace ino */


