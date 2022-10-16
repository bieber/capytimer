firmware: main.c write_pixels.S write_pixels.h
	avr-gcc -Os -mmcu=attiny44 write_pixels.S main.c -o firmware

firmware.hex: firmware
	avr-objcopy -j .text -j .data -O ihex firmware firmware.hex

program: firmware.hex
	avrdude -c usbasp -p t44 -U flash:w:firmware.hex:i

fuse:
	avrdude -c usbasp -p t44 -B1000 -U lfuse:w:0xFF:m
