#include "commands/digitalwrite.h"
int ino::cmd_digitalwrite(Span<StringView<>> argv) {
	const auto* pin = pincommand_check(argv, 3);
	if(not pin) {
		return -1;
	}
	LogicLevel logic_level = LogicLevel::Low;
	if(argv[2] == "low" or argv[2] == "LOW" or argv[2] == "0") {
		logic_level = LogicLevel::Low;
	} else if(argv[2] == "high"  or argv[2] == "HIGH" or argv[2] == "1") {
		logic_level = LogicLevel::High;
	} else {
		return command_error(F("Invalid logic level '"), argv[2], F("'."));
	}
	auto err = pin->digital_write(logic_level);
	if(err != PinStatus::Good) {
		return command_error(F("Pin "), argv[1], F(" is not currently in OUTPUT mode."));
	}
	return 0;
}

