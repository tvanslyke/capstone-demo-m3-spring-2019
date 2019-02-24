release/firmware.hex: release/firmware.elf
	echo "Converting to firmware.hex"
	avr-objcopy -O ihex -R .eeprom release/firmware.elf release/firmware.hex

firmware:
	cd src/ && $(MAKE)
	

release/firmware.elf: firmware
	cp src/firmware.elf release/firmware.elf

arduino/libarduino.a: ./arduino/*.h ./arduino/*.cpp ./arduino/*.c
	cd arduino/ && $(MAKE)

install: release/firmware.elf
	avrdude -v -D -patmega328p -carduino -b57600 -C/usr/share/arduino/hardware/tools/avrdude.conf -P/dev/ttyUSB0 -Uflash:w:release/firmware.hex:i
	# avrdude -v -D -patmega328p -carduino -b115200 -C/usr/share/arduino/hardware/tools/avrdude.conf -P/dev/ttyACM0 -Uflash:w:release/firmware.hex:i

serial:
	# picocom --echo --omap=crlf --baud=115200 /dev/ttyUSB0
	python3 serialcomm.py

clean:
	cd src/ && $(MAKE) clean
	rm release/firmware.elf
	rm release/firmware.hex
