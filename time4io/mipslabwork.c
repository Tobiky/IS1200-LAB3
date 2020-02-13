/* mipslabwork.c

   This file written 2015 by F Lundevall
   Updated 2017-04-21 by F Lundevall

   This file should be changed by YOU! So you must
   add comment(s) here with your name(s) and date(s):

   This file modified 2017-04-31 by Ture Teknolog

   For copyright and licensing, see file COPYING */

#include <stdint.h>   /* Declarations of uint_32 and the like */
#include <pic32mx.h>  /* Declarations of system-specific addresses etc */
#include "mipslab.h"  /* Declatations for these labs */


// All TRISx (replace x with some port name, for example port E: TRISE) registers change 
// if a pin (can be viewed as a device in some cases to make it easier) is in input/output mode depending on the bit.
// Example: Bit 0 (first bit) for port E is the first LED on the shield.
// How many bits controll what or do what depends on the port, if you are unsure check the chipKIT Uno32 manuals.

// Since TRISE and PORTE are already defined in pic32mx.h the pointers names for respective registers begin with an underscore ( _ ).
// They are set to volatile so no compile optimizations happen to them, for example setting them as constant values somewhere
// when they in fact might change.

// As the instructions tell us we create [volatile] pointers to the respective ports with the given register memory adresses.
volatile int* _TRISE = (volatile int*)0xbf886100;
volatile int* _PORTE = (volatile int*)0xbf886110;


int mytime = 0x5957;

char textstring[] = "text, more text, and even more text!";

/* Interrupt Service Routine */
void user_isr(void)
{
	return;
}

/* Lab-specific initialization goes here */
void labinit(void)
{ 
    // TRISE bits 0-7 controll the LEDs on the shield. We set these bits to 0 to mark them as outputs.
    
    // *: Putting a * in front of a pointer gives us the value from that memory. This is called [dereferencing].
    //    Note that in C the memory covarage is handled through the pointer type. That is; if the pointer type is an int (int*) 
    //    the value from dereferencing will cover 4 bytes of memory, and adding to the pointer (aka adding to the adress)
    //    will increase in units of 4 bytes. += 1 will be handled as += 4 for the adress.

    // &=: "x &= y" will be replaced by "x = x & y" when the program is compiled.
    //     & is called bitwise-and because it performs an and operation on each bit seperatly (and gives the resulting bit sequence)
    //     1101 & 0110 = 0100

    // ~: The ~ operator is called bitwise invert and just as it the name suggest it inverts all the bits, 
    //    1101 will become 0010

    // These three operators are used to make the first 8 bits of TRISE to become 0 and leave the rest untouched (we mask with 8 zeroes 
    // at the least significant part). That is, we make all devices connected to the 8 first bits act as outputs.

	*_TRISE &= ~0xff;
	
    // All registers also have set registers. These registers set a 1 to the specified bit. 
    //  * xSET = 0x1 will set 1 to bit 1 of register x (0x1 = 001)
    //  * xSET = 0x4 will set 1 to bit 3 of register x (0x4 = 100)
    //  * xSET = 0x5 will set 1 to bit 3 and 1 of register x (0x5 = 101)

    // As the instructions tell us we set bits 5-11 of TRISD to 1 (making devices at port D on bits 5-11 inputs)
    // by using the set register. 0x0fe0 = 0000 1111 1110 0000 
    TRISDSET = 0x0fe0;
}

/* This function is called repetitively from the main program */
void labwork(void)
{
    // We get the state of the buttons and put them inside a variable as the state will be reused in several parts
    // inside the if statement.
	int btns = getbtns();

    // As C treats 0 as false and anything else as true, as long as any button has a state the value will
    // be treated as true.
	if (btns) {
        // Same as with the buttons, we save the state as it will be reused.
		int swt = getsw();
		
        // BTN2 (Button 2) is on the first bit, and the others follow, as per the specifications in the instructions for getbtns().
		if (btns & 0x1)
			// We mask mytime so the specific slot only has the bits we OR into place.
            // The masked off bits are replaced with the state of the switches by shifting those values into the correct values.
            // As the values are shifted in 4's we can make it easier for ourselves by writing it in mulitples of 4.
            mytime = (mytime & 0xff0f) | (swt << (1 * 4));

        // BTN3
		if (btns & 0x2)
            // The same as for BTN2 but a different slot and different shift value
			mytime = (mytime & 0xf0ff) | (swt << (2 * 4));

        // BTN4
		if (btns & 0x4)
            // The same as for BTN2 but a different slot and different shift value
			mytime = (mytime & 0x0fff) | (swt << (3 * 4));
	}

    // Subroutines from previos lab assignments and display functions
	delay(1000);
	time2string(textstring, mytime);
	display_string(3, textstring);
	display_update();
	tick(&mytime);
	display_image(96, icon);

    // The instructions ask us to update the LEDs with binary increases
    // but as the computer already uses binary as its format and the port E displays the very same format.

    // That is, the register port E inside the computer is something like 0001 0010 1001 01001 0000 11101 1001 1000
    // Those 8 first bits are the ones that decide which leds are on or off

    // If we think of them as starting on all 0 and increase in binary form, we get
    // 0000 0000 -> 0000 0001 -> 0000 0010 -> 0000 0011 -> etc

    // Which already performs the function asked from the instructions. As such we only increase the value of port E.
	(*_PORTE)++;
}
