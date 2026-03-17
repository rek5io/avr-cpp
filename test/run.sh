#!/usr/bin/bash

src="src"
build="build"
main="main.cpp"

avr-g++ -mmcu=atmega328p -O3 -o $build/main.elf $src/$main
avr-objcopy -O ihex -R .eeprom $build/main.elf $build/main.hex
avrdude -v -c arduino -P /dev/ttyUSB0 -b 57600 -p atmega328p -D -U flash:w:$build/main.hex:i
