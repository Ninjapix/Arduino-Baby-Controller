# Arduino-Baby-Controller
Converting a baby toy shaped like a controller into a functional gamecube controller.

Video of the controller working: https://youtu.be/uSsChrTA_CA

Too lazy to write an official build guide for the V1, so here's the jist:
1. Clear out the battery compartment and make a hole
2. Take out the joystick button and desolder it from the board.
3. Attach wires to all the test pads, plus ground.
4. Wire those up to the Arduino nano
5. Wire up the joystick pads to the nano, this will be used as "rumble"
6. Place the joystick button by the lightswitch bumper, and wire those up to the arduino like a standard button. Remember: this is a pull up button.
7. Place the control stick module into the controller
8. Use hotglue to get it to the perfect length away from the shell
9. Wire the module up to the controller
10. Take a gamecube extension cable and wire it up to the bi-directional level converter
11. Wire the 5V power supply from the gamecube wire to the arduino + the 3.3V converter (i think i dont really remember, you probably shouldn't)
12. Wire the gamecube data wires from the bi-directional converter to the nano
13. Put 2 Dip Switch modules into where the original power switch was, and solder the legs to the nano
14. cry because you spent like a month or two on this dumb project 
