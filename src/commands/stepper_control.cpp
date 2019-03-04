#include <Arduino.h>
#include "commands/stepper_control.h"
#include "Stepper.h"
#include "ino_assert.h"

static ino::Stepper<100u, 4, 7, 5, 6> stepper;


int ino::cmd_window(Span<StringView<>> argv) {
	static bool initialized = false;
	if(not initialized) {
		stepper.begin();
		initialized = true;
	}
	switch(argv.size()) {
	default:
		return ino::command_error(F("Command 'window' takes at most one argument"));
	case 1:
		switch(stepper.position()) {
		case 0u:
			Serial.println(F("CLOSED"));
			break;
		case 50u:
			Serial.println(F("OPENED"));
			break;
		}
		return 0;
	case 2: 
		if(argv[1] == "OPEN" or argv[1] == "open") {
			if(stepper.position() == 0u) {
				stepper.set_position(25u);
				stepper.set_position(50u);
			} else {
				ASSERT(stepper.position() == 50u);
			}
			return 0;
		} else if(argv[1] == "CLOSE" or argv[1] == "close") {
			if(stepper.position() == 50) {
				stepper.set_position(25u);
				stepper.set_position(0u);
			} else {
				ASSERT(stepper.position() == 0u);
			}
			return 0;
		} else {
			return command_error(F("Invalid argument to command 'window'.  Valid values are 'OPEN', 'open', 'CLOSE', or 'close'."));
		}
	}
}
