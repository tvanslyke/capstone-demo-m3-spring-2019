#include "commands/headlights.h"

int ino::cmd_headlights(Span<StringView<>> argv) {
	constexpr int8_t num = 9;
	pin<num>.set_mode(PinMode::Output);
	switch(argv.size()) {
	default:
		return command_error(F("Command 'headlights' takes at most 1 argument."));
	case 2:
		if(argv[1] == "ON" or argv[1] == "on" or argv[1] == "1") {
			(void)pin<num>.digital_write(LogicLevel::High);
		} else if(argv[1] == "OFF" or argv[1] == "off" or argv[1] == "0") {
			(void)pin<num>.digital_write(LogicLevel::Low);
		} else {
			return command_error(F("Expected one of 'on', 'ON', 'off', or 'OFF'."));
		}
		break;
	case 1:
		(void)0;
	}
	// Echo the current status of the pin.
	if(digitalRead(9) == HIGH) {
		Serial.println(F("ON"));
	} else {
		Serial.println(F("OFF"));
	}
	return 0;

}

