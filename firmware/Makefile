C_SOURCES :=$(wildcard *.c)
ASM_SOURCES :=$(wildcard *.S)
HEADERS :=$(wildcard *.h)

firmware: $(C_SOURCES) $(ASM_SOURCES) $(HEADERS)
	avr-gcc -Os -mmcu=attiny44 $(C_SOURCES) $(ASM_SOURCES) -o firmware

firmware.hex: firmware
	avr-objcopy -j .text -j .data -O ihex firmware firmware.hex

program: firmware.hex
	avrdude -c usbasp -p t44 -U flash:w:firmware.hex:i

fuse:
	avrdude -c usbasp -p t44 -B1000 -U lfuse:w:0xFF:m
