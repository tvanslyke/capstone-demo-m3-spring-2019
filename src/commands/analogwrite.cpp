#include "commands/analogwrite.h"

int ino::cmd_analogwrite(Span<StringView<>> argv) {
	const auto* pin = pincommand_check(argv, 3);
	if(not pin) {
		return -1;
	}
	Optional<long> value = parse_decimal<long>(argv[2]);
	if(not value) {
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

