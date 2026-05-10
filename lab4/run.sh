#!/usr/bin/bash

rm -rf build
mkdir -p build

avr-g++ -mmcu=atmega328p -Os -DF_CPU=16000000UL  -o build/main.elf src/*.cpp
avr-size --format=avr --mcu=atmega328p build/main.elf
avr-objcopy -O ihex -R .eeprom build/main.elf build/main.hex
avrdude -v -c arduino -P /dev/ttyUSB0 -b 57600 -p atmega328p -D -U flash:w:build/main.hex:i
