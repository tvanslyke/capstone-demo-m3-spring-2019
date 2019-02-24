# ARCHITECTURE = avr
# BOARD_TAG = atmega328
# include /usr/share/arduino/Arduino.mk


#BSD licensed, see http://lastyearswishes.com/static/Makefile for full license

HDRS = Arduino.h binary.h Client.h HardwareSerial.h IPAddress.h new.h pins_arduino.h Platform.h Printable.h Print.h \
	Server.h Stream.h Udp.h USBAPI.h USBCore.h USBDesc.h WCharacter.h wiring_private.h WString.h


OBJS = WInterrupts.o wiring_analog.o wiring.o wiring_digital.o wiring_pulse.o wiring_shift.o CDC.o HardwareSerial.o \
	HID.o IPAddress.o new.o Print.o Stream.o Tone.o USBCore.o WMath.o WString.o

#may need to adjust -mmcu if you have an older atmega168
#may also need to adjust F_CPU if your clock isn't set to 16Mhz
CFLAGS = -I./ -std=gnu99  -DF_CPU=16000000UL -Os -mmcu=atmega328p
CPPFLAGS = -I./ -DF_CPU=16000000UL -Os -mmcu=atmega328p

CC=avr-gcc-8
CPP=avr-g++-8
AR=avr-ar


default: libarduino.a

libarduino.a:   ${OBJS}
	${AR} crs libarduino.a $(OBJS)

.c.o: ${HDRS}
	${CC} ${CFLAGS} -c $*.c

.cpp.o: ${HDRS}
	${CPP} ${CPPFLAGS} -c $*.cpp

clean:
	rm -f ${OBJS} core a.out errs

install: libarduino.a
	mkdir -p ${PREFIX}/lib
	mkdir -p ${PREFIX}/include
	cp *.h ${PREFIX}/include
	cp *.a ${PREFIX}/lib
