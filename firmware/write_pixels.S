/*
 * Copyright (C) 2022, Robert Bieber
 *
 * This program is free software: you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see
 * <https://www.gnu.org/licenses/>.
 */

#define __SFR_OFFSET 0
#include <avr/io.h>

#include "pins.h"

;; void write_pixels(uint8_t *pixels, uint16_t count)
;; pixels: r24
;; count: r22:r23
;;
;; One serial cycle is 25 CPU cycles @ 20MHz
;; High/low times in cycles:
;;
;;   1 bit - 16 high, 9 low
;;   0 bit - 8 high, 17 low
;;
;; r18 - Current byte value
;; r19 - Bit counter
;; r20 - Saved SREG
;; r21 - Fast low output
;; r22 - Fast high output
;; r24:25 - Pixel count
;; r26:27 - Input buffer (X)
;; r30:31 - Return jump location (Z)
.global write_pixels
write_pixels:
	movw r26, r24				; Moving the data pointer into X
	movw r24, r22				; Moving the count into the range for sbiw
	in r20, SREG				; Preserving status register
	cli							; Disable interrupts for cycle counting

	in r21, PORT_SCREEN			; PORT_SCREEN with PIN_SCREEN turned off
	andi r21, ~(1 << PIN_SCREEN)

	in r22, PORT_SCREEN			; PORT_SCREEN with PIN_SCREEN turned on
	ori r22, (1 << PIN_SCREEN)

	ld r18, X+
	ldi r19, 8					; 8 bits per byte

begin:
	out PORT_SCREEN, r22		; Output high, t=0

	lsl r18						; Rotate MSB into C, t=1
	brcs handle_1				; Branch based on bit

handle_0:
	;; t=2
	nop							; t=3
	nop							; t=4
	nop							; t=5
	nop							; t=6
	nop							; t=7
	out PORT_SCREEN, r21		; Output low, t=8
	dec r19						; Decrement bit counter, t=9
	breq next_byte_0			; If we're out of bits, decrement byte counter
	;; t=10
	nop							; t=11
	nop							; t=12
	nop							; t=13
	nop							; t=14
	nop							; t=16
	rjmp same_byte_0			; t=17
next_byte_0:
	;; t=11
	sbiw r24, 1					; t=13
	breq end					; If we're out of bytes, return, t=14
	ld r18, X+					; Load the next byte, t=16
	ldi r19, 8					; Reset bit count, t=17
same_byte_0:
	;; t=17
	nop							; t=18
	nop							; t=19
	nop							; t=20
	nop							; t=21
	nop							; t=22
	nop							; t=23
	rjmp begin					; t=25

handle_1:
	;; t=3
	dec r19						; Decrement bit counter, t=4
	breq next_byte_1			; If we're out of bits, decrement byte counter
	;; t=5
	nop							; t=6
	nop							; t=7
	nop							; t=8
	rjmp same_byte_1			; t=10
next_byte_1:
	;; t=6
	sbiw r24, 1					; Decrement byte counter, t=7
	ld r18, X+					; Load next byte (or garbage), t=9
	ldi r19, 8					; Reset bit count, t=10
same_byte_1:
	;; t=10
	nop							; t=11
	nop							; t=12
	nop							; t=13
	nop							; t=14
	nop							; t=15
	out PORT_SCREEN, r21		; Output low, t=16
	breq end					; If we're out of bytes, return, t=17
	nop							; t=18
	nop							; t=19
	nop							; t=20
	nop							; t=21
	nop							; t=22
	nop							; t=23
	rjmp begin					; t=25

end:
	out SREG, r20				; Restore interrupt state
	ret
