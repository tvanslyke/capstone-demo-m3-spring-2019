CC=avr-gcc-8
CXX=avr-g++-8
AR=avr-ar

DEVICE=__AVR_ATmega328P__

CXXFLAGS=-mmcu=atmega328p -std=c++17 -O2 -fmax-errors=5 -Wall -Wextra -DF_CPU=16000000L -DARDUINO=105 -ffunction-sections -fdata-sections -w -I./../arduino/ -I./ArduinoSTL/src -I./
#  -I/usr/share/arduino/hardware/arduino/variants/standard -I/usr/share/arduino/hardware/arduino/cores/arduino
# -D$(DEVICE) 

OBJECTS=main.o Command.o Pins.o digitalwrite.o digitalread.o analogwrite.o analogread.o pinmode.o headlights.o checkengine.o stepper_control.o

firmware.elf: $(OBJECTS)
	$(CXX) $(OBJECTS) ./../arduino/libarduino.a $(CXXFLAGS) -o firmware.elf

./../arduino/libarduino.a:
	cd ./../arduino && $(MAKE)

Pins.o: Pins.cpp Pins.h Command.h ino_assert.h Array.h ./ArduinoSTL/src/*.h
	$(CXX)  Pins.cpp $(CXXFLAGS) -c 

Command.o: Command.cpp Command.h ./ArduinoSTL/src/*.h IteratorRange.h Pins.h ino_assert.h
	$(CXX)  Command.cpp $(CXXFLAGS) -c 

pinmode.o: commands/pinmode.h commands/pinmode.cpp Command.h
	$(CXX)  commands/pinmode.cpp $(CXXFLAGS) -c 

digitalread.o: commands/digitalread.h commands/digitalread.cpp Command.h
	$(CXX)  commands/digitalread.cpp $(CXXFLAGS) -c 

digitalwrite.o: commands/digitalwrite.h commands/digitalwrite.cpp Command.h
	$(CXX)  commands/digitalwrite.cpp $(CXXFLAGS) -c 

analogread.o: commands/analogread.h commands/analogread.cpp Command.h
	$(CXX)  commands/analogread.cpp $(CXXFLAGS) -c 

analogwrite.o: commands/analogwrite.h commands/analogwrite.cpp Command.h
	$(CXX)  commands/analogwrite.cpp $(CXXFLAGS) -c 

headlights.o: commands/headlights.h commands/headlights.cpp Command.h
	$(CXX)  commands/headlights.cpp $(CXXFLAGS) -c 

checkengine.o: commands/checkengine.cpp commands/checkengine.h Command.h ./ArduinoSTL/src/*.h
	$(CXX)  commands/checkengine.cpp $(CXXFLAGS) -c 

stepper_control.o: commands/stepper_control.cpp commands/stepper_control.h Command.h Stepper.h ./ArduinoSTL/src/*.h
	$(CXX)  commands/stepper_control.cpp $(CXXFLAGS) -c 

main.o: main.cpp Command.h command_parsing.h ino_assert.h
	$(CXX) main.cpp -c $(CXXFLAGS) 

clean:
	rm ./*.o
	rm firmware.elf
