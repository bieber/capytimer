# CapyTimer

CapyTimer is meant to be a simple timer for
boxing/kickboxing/grappling/fitness.  My goal is to keep its interface
as simple as possible while maintaining the most important
functionality.  Work and rest times are set with dials, and the timing
started and stopped with a switch or button.  The display counts the
remaining time, the current round, and nothing else.

The display is constructed from WS2812B addressable LEDs glued in to
3D printed seven segment displays.  The digits and controls can be
mounted in a 3D printed panel or a CNC (or hand) cut wood panel.  I'll
add some basic assembly instructions here as the project progresses,
for now it's very much a work in progress.  The current directory
layout is:

* `3d/` - Files for 3D printing
* `firmware/` - Code to run the AVR microcontroller
