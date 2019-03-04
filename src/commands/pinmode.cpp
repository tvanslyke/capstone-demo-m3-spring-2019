#include "commands/pinmode.h"

int ino::cmd_pinmode(Span<StringView<>> argv) {
	if(auto* pin = pincommand_check(argv, 2, 3); not pin) {
		return -1;
	} else if(argv.size() == 2) {
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
	} else if(argv[2] == "INPUT" or argv[2] == "input") {
		pin->set_mode(PinMode::Input);
		return 0;
	} else if(argv[2] == "OUTPUT" or argv[2] == "output") {
		pin->set_mode(PinMode::Output);
		return 0;
	} else if(argv[2] == "INPUT_PULLUP" or argv[2] == "input_pullup") {
		pin->set_mode(PinMode::InputPullup);
		return 0;
	} else {
		return command_error(F("Invalid pin mode '"), argv[2], "'.");
	}
}

