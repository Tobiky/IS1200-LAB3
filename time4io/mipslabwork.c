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
	int btns;
	if ((btns = getbtns())) {
		int swt = getsw();
		
		if (btns & 0x1)
			mytime = (mytime & 0xff0f) | (swt << (1 * 4));

		if (btns & 0x2)
			mytime = (mytime & 0xf0ff) | (swt << (2 * 4));

		if (btns & 0x4)
			mytime = (mytime & 0x0fff) | (swt << (3 * 4));
	}

	*_PORTE &= 0x0ff;
	delay(1000);
	time2string(textstring, mytime);
	display_string(3, textstring);
	display_update();
	tick(&mytime);
	display_image(96, icon);
	(*_PORTE)++;
}

//QUESTIONS
//The third and second didgit changes. Because we have an if for if any button is pushed
//TRISE = the value, TRISESET = set the value (index + 1), TRISECLR = clear the value (index + 1)
//v0
//?????
