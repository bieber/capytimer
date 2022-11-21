C_SOURCES :=$(wildcard *.c)
ASM_SOURCES :=$(wildcard *.S)
HEADERS :=$(wildcard *.h)

firmware: $(C_SOURCES) $(ASM_SOURCES) $(HEADERS)
	avr-gcc -Os -mmcu=atmega328p $(C_SOURCES) $(ASM_SOURCES) -o firmware

firmware.hex: firmware
	avr-objcopy -j .text -j .data -O ihex firmware firmware.hex

program: firmware.hex
	avrdude -c usbasp -p m328p -U flash:w:firmware.hex:i

fuse:
	avrdude -c usbasp -p m328p -B1000 -U lfuse:w:0xF7:m
