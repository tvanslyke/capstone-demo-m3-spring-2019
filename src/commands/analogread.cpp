#include "commands/analogread.h"

int ino::cmd_analogread(Span<StringView<>> argv) {
	const auto* pin = pincommand_check(argv, 2);
	if(not pin) {
		return -1;
	}
	auto [value, status]= pin->analog_read();
	if(status == PinStatus::BadPinKind) {
		return command_error("Pin ", argv[1], F(" is not an analog pin."));
	} else if(status == PinStatus::BadPinMode) {
		return command_error("Pin ", argv[1], F(" is not in INPUT or INPUT_PULLUP mode."));
	} else if(status != PinStatus::Good) {
		return command_error(F("Unable to read from pin "), argv[1], ".");
	}
	Serial.println(value);
	return 0;
}

