#include <Arduino.h>

#include "Command.h"
#include "command_parsing.h"


void setup()
{
	Serial.begin(115200);
	Serial.println("Initializing...");
}

static char line_buffer[128] = "";
static char* token_buffer[10] = {nullptr};


void loop()
{
	Serial.print("ino> ");
	auto line_length = ino::read_line(Serial, line_buffer);
	if(line_length < 0) {
		Serial.println("Error: Command too long.");
		return;
	}
	int count = ino::tokenize_line(token_buffer, line_buffer, line_length);
	if(count < 0) {
		Serial.println("Error: Too many tokens in command.");
		return;
	}
	int err = ino::invoke_command(count, token_buffer);
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

