/* mipslabwork.c

   This file written 2015 by F Lundevall
   Updated 2017-04-21 by F Lundevall

   This file should be changed by YOU! So you must
   add comment(s) here with your name(s) and date(s):

   This file modified 2017-04-31 by Ture Teknolog
   
   Modified 2020-02-13 by A Hammarstrand

   For copyright and licensing, see file COPYING */

#include <stdint.h>   /* Declarations of uint_32 and the like */
#include <pic32mx.h>  /* Declarations of system-specific addresses etc */
#include "mipslab.h"  /* Declatations for these labs */

volatile int* _TRISE = (volatile int*)0xbf886100;
volatile int* _PORTE = (volatile int*)0xbf886110;

int timeoutcount = 0;
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
    // Same as in time4io/mipslabwork.c but we use the set register instead of pointers, as they
    // are unnecessary now. 
	TRISESET = ~0xff;
	TRISDSET = 0xfe0;


    // There exists a total of two IFS registers, as you can see in the PIC32MX Family Data Sheet 
    // page 51 (https://ww1.microchip.com/downloads/en/DeviceDoc/61143H.pdf)
     
    // In the Timer Section manual (https://ww1.microchip.com/downloads/en/DeviceDoc/61105F.pdf), 
    // "Control Registers" chapter, we can see that there are three relevant registers; 
    // TxCON (Timer x Controll Register), TMRx (Timer x Timer Value), and PRx (Timer x Period Register) 
    // (replace "x" with the timer number). 
    
    // As the instructions tell us to use timer 2, T1CON is irrelvant.
    // TxCON holds most of the configuration for the timer, but as for as we are concerned only bits 4-6 and 15 matters. 
    // When bit 15 is 1 the timer is on and vice versa. We will come to bits 4-6 later in this function.

    
    // We set bit 15 to 0 for timer 2, so that it is not running while we change everything and we start off with correct values.
    // The bit may already be set to 0 when the program starts, however we can not assume this and thus enforce it.
    T2CONCLR  = 1 << 15;  // Turn the timer off


    // TMx holds the value of timer x, so TM2 holds the value of timer 2. 
    // This value is set to 0 so we can be sure the timer starts at 0.
    TMR2       = 0; // Reset timer value

    // The prescaling for the timer is also part of TxCON, specifically bits 4-6 (0xxx 0000, x are the prescaling bits).
    // As the frequency (or speed) of the clock is at 80 MHz, we know that it will tick
    // 80 million times in 1 second. However, we know, from the lecture slides, that the period
    // is not big enough to hold that number, so we use the largest prescaling to get as close to
    // 1 s we can.

    // This gives us 80 000 000 / 256 = 312 500 for 1s. Because we will devide the second further,
    // we now know the period register is large enough to hold the period value.
    T2CONSET   = 0x70;           // Set prescaling to 111 -> 1:256 (0111 0000)
    
    // PRx holds the period of timer x, so PR2 holds the period of timer 2.
    // As the period is currently 312 500 for 1s and we know that we want a period
    // on 0.1s (100 ms) we can just devide the value by 10 and get the period value
    // we want:
    // 312 500 / 10 = 31 250 
    
    // As you might have noticed it's also possible to set the prescaling to 1:128 (110)
    // since the register is 16 bits and [2^16 - 1 = 65 535] and [31 250 * 2 = 62 500].
    PR2        = 31250;          // Set period to 80 000 000 / (256 * 10) (1s/10 = 100ms) 

    // In the Timer Section manual, in chapter "Control Registers", we can see the mention of
    // "Interrupt Status Flag bit". This is the bit that tells us if the timer has
    // reached the period or not. This part also tells us the name of this bit, namely "TxIF".
    // The x, as usual, is for the timer. The acronym reads out "Timer x Interrupt Flag".
    // As we are unsure what the register is or how it's been used, we reset it to make sure
    // we get the behavior we want; the timer starting from 0.

    // Further the manual tells us that it lies in IFS0 by "in IFS0 interrupt register".
    // The Pic Family Data Sheet, on page 53, holds this information. We look for IFS0
    // in the second column from the left. This row is further devided into two rows;
    // 31:16 and 15:0, these are for the bits. All names that lie on the 31:16 row
    // are in the 31:16 bit range of that register. The top row shows 31/15, 30/14, etc
    // this is the specific bit. If we look for T2IF as we mentioned above we can see
    // that it is in 15:0 range and that the bit is the left of 24/8, e.g. bit 8.

    // We reset this flag to make sure it starts out at 0.
    IFSCLR(0)  = 1 << 8;         // Reset interrupt flag
  
    // As you might have noticed from looking at the data sheet, there are several more
    // slots filled. Each of these are different devices. Through the manuals you can
    // find which ones belong to what.
   
    // As you can see we use bit 15 again, but this time we set it to 1 to turn the timer on.
    T2CONSET   = 1 << 15;        // Set timer 2 to ON 
}

/* This function is called repetitively from the main program */
void labwork(void)
{
    // Review the comments in time4io/mipslabwork.c if you are confused about this part
    int btns = getbtns();
	if (btns) {
		int swt = getsw();
		
		if (btns & 0x1)
			mytime = (mytime & 0xff0f) | (swt << (1 * 4));

		if (btns & 0x2)
			mytime = (mytime & 0xf0ff) | (swt << (2 * 4));

		if (btns & 0x4)
			mytime = (mytime & 0x0fff) | (swt << (3 * 4));
	}

    // The buttons, as per instructions, are supposed to act indenpendantly and thus they
    // they are not included in the below if-statement.

    // From what we've learned from the labinit function, we get interrupt status flag 
    // for timer 2 and only this. Because anything other than 0 is true, we know that
    // if the flag is set it will not be 0, therefore it will be true. If the flag is not
    // set this expression will only become 0, therefore false.
    if (IFS(0) & (1 << 8)) {
        // Clear the flag so that we know of the timer reaching its period again.
        IFSCLR(0) = 1 << 8;

        // As the instructions tell us we need to increase 'timeoutcount' every 100ms.
        timeoutcount++;
    }

    // The instructions told us to only do the this update when timeoutcount reaches 10.
    if (timeoutcount == 10) {
        // We resest timeoutcount to 0 so that it can reach 10 again.
        timeoutcount = 0;

        // This is just the previous parts.
        PORTESET = 0x0ff;
        
        time2string(textstring, mytime);
        display_string(3, textstring);
        display_update();
        tick(&mytime);
        display_image(96, icon);
        
        (PORTE)++;
	}
}
