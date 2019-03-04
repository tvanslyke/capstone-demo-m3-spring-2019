#include "commands/digitalread.h"


int ino::cmd_digitalread(Span<StringView<>> argv) {
	auto* pin = pincommand_check(argv, 2);
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

