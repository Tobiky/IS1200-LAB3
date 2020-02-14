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
    T2CONCLR   = 1 < 15  // Turn the timer off

    // TMx holds the value of timer x, so TM2 holds the value of timer 2. 
    // This value is set to 0 so we can be sure the timer starts at 0.
    TMR2       = 0; // Reset timer value

    // IMPROVE PRESCALING ARGUMENT
    // The prescaling for the timer is also part of TxCON, specifically bits 4-6.
    // As the frequency (or speed) of the clock is at 80 MHz, we know that it will tick
    // 80 million times in 1 second. But we know, from the lecture slides, that the period
    // is not big enough to hold that number, so we use the largest prescaling since 100 ms = 0.1s
    T2CONSET   = 0x70;           // Set prescaling to 111 -> 1:256
    
    // PRx holds the period of timer x, so PR2 holds the period of timer 2.
    // 
    PR2        = 31250;          // set period to 80 000 000 / (256 * 10) (1s/10 = 100ms) 
    IFSCLR(0)  = 1 << 8;       // reset interrupt flag
    T2CONSET   = 1 << 15;      // set timer 2 to ON 
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

    // IFS is an acronym for Interrupt Flag Status. Each bit is a different flag - which matches for a different register.
    if (IFS(0) & (1 << 8)) {
        IFSCLR(0) = 1 << 8;    
        timeoutcount++;
    }

    if (timeoutcount == 10) {
        timeoutcount = 0;

        *_PORTE &= 0x0ff;
        
        time2string(textstring, mytime);
        display_string(3, textstring);
        display_update();
        tick(&mytime);
        display_image(96, icon);
        
        (*_PORTE)++;
	}
}

//QUESTIONS
//The third and second didgit changes. Because we have an if for if any button is pushed
//TRISE = the value, TRISESET = set the value (index + 1), TRISECLR = clear the value (index + 1)
//v0
//?????
