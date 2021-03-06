#include <Arduino.h>

#include "Pins.h"
#include "Command.h"
#include "command_parsing.h"
#include "commands/checkengine.h"


void setup()
{
	Serial.begin(115200);
	Serial.println("Initializing...");
	ino::pin< 2>.set_mode(ino::PinMode::Input);
	ino::pin< 3>.set_mode(ino::PinMode::Input);
	ino::pin< 4>.set_mode(ino::PinMode::Input);
	ino::pin< 5>.set_mode(ino::PinMode::Input);
	ino::pin< 6>.set_mode(ino::PinMode::Input);
	ino::pin< 7>.set_mode(ino::PinMode::Input);
	ino::pin< 8>.set_mode(ino::PinMode::Input);
	ino::pin< 9>.set_mode(ino::PinMode::Input);
	ino::pin<10>.set_mode(ino::PinMode::Input);
	ino::pin<11>.set_mode(ino::PinMode::Input);
	ino::pin<12>.set_mode(ino::PinMode::Input);
	ino::pin<13>.set_mode(ino::PinMode::Input);
	ino::pin<A0>.set_mode(ino::PinMode::Input);
	ino::pin<A1>.set_mode(ino::PinMode::Input);
	ino::pin<A2>.set_mode(ino::PinMode::Input);
	ino::pin<A3>.set_mode(ino::PinMode::Input);
	ino::pin<A4>.set_mode(ino::PinMode::Input);
	ino::pin<A5>.set_mode(ino::PinMode::Input);
	ino::pin<A6>.set_mode(ino::PinMode::Input);
	ino::pin<A7>.set_mode(ino::PinMode::Input);
	attachInterrupt(0, ino::checkengine_interrupt, CHANGE);
}

// Buffer to read lines from serial into.
static char line_buffer[128] = "";
// Buffer to store tokens in when tokenizing lines.
static ino::StringView<> token_buffer[10] = {{}};

void loop()
{
	Serial.print("ino> ");
	auto line_length = ino::read_line(Serial, line_buffer);
	if(line_length < 0) {
		Serial.println("Error: Command too long.");
		return;
	}
	int count = ino::tokenize_line(token_buffer, ino::StringView<>(line_buffer, line_length));
	if(count < 0) {
		Serial.println("Error: Too many tokens in command.");
		return;
	}
	int err = ino::invoke_command(ino::Span(token_buffer, count));
}

int main(void)
{
	init();

#if defined(USBCON)
	USBDevice.attach();
#endif
	
	setup();
    
	for (;;) {
		loop();
		if (serialEventRun) serialEventRun();
	}
        
	return 0;
}

