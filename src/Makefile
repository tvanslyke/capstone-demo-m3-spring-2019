CC=avr-gcc-8
CXX=avr-g++-8
AR=avr-ar

DEVICE=__AVR_ATmega328P__

CXXFLAGS=-mmcu=atmega328p -std=c++17 -Os -fmax-errors=5 -Wall -Wextra -DF_CPU=16000000L -DARDUINO=105 -ffunction-sections -fdata-sections -w -I./../arduino/ -I./ArduinoSTL/src
#  -I/usr/share/arduino/hardware/arduino/variants/standard -I/usr/share/arduino/hardware/arduino/cores/arduino
# -D$(DEVICE) 

OBJECTS=main.o Command.o stepper_control.o

firmware.elf: $(OBJECTS)
	$(CXX) $(OBJECTS) ./../arduino/libarduino.a $(CXXFLAGS) -o firmware.elf

./../arduino/libarduino.a:
	cd ./../arduino && $(MAKE)

stepper_control.o: stepper_control.cpp stepper_control.h Command.h Stepper.h ./ArduinoSTL/src/*.h
	$(CXX)  stepper_control.cpp $(CXXFLAGS) -c 

Command.o: Command.cpp Command.h ./ArduinoSTL/src/*.h IteratorRange.h Pins.h ino_assert.h
	$(CXX)  Command.cpp $(CXXFLAGS) -c 

main.o: main.cpp Command.h command_parsing.h ino_assert.h
	$(CXX) main.cpp -c $(CXXFLAGS) 

clean:
	rm ./*.o
	rm firmware.elf
