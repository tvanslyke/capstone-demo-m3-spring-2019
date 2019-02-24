#include <Arduino.h>
#include "stepper_control.h"
#include "Stepper.h"

static ino::Stepper<100u, 4, 7, 5, 6> stepper;


int ino::cmd_stepper(int argc, char** argv) {
	static bool initialized = false;
	if(not initialized) {
		stepper.begin();
		initialized = true;
	}
	switch(argc) {
	default:
		return ino::command_error(F("Command 'stepper' takes at most one argument"));
	case 1:
		Serial.println(stepper.position());
		return 0;
	case 2: 
		if(long value = std::strtol(argv[1], nullptr, 10); value == 0 and argv[1][0] != '0') {
			return ino::command_error(F("Cannot parse '"), argv[1], F("' as a decimal integer in command 'stepper'."));
		} else if(value < 0 or value > 99) {
			return ino::command_error(F("A value between 0 and 99 is required."));
		} else {
			stepper.set_position(static_cast<uint8_t>(value));
			return 0;
		}
	}
}
