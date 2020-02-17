/* mipslabwork.c

   This file written 2015 by F Lundevall
   Updated 2017-04-21 by F Lundevall

   This file should be changed by YOU! So you must
   add comment(s) here with your name(s) and date(s):

   This file modified 2017-04-31 by Ture Teknolog

   Modified by A Hammarstrand 2020-02-17
   For copyright and licensing, see file COPYING */

#include <stdint.h>   /* Declarations of uint_32 and the like */
#include <pic32mx.h>  /* Declarations of system-specific addresses etc */
#include "mipslab.h"  /* Declatations for these labs */

int prime = 1234567;

int timeoutcount = 0;
int mytime = 0x5957;

char textstring[] = "text, more text, and even more text!";

/* Interrupt Service Routine */
void user_isr(void)
{
    // As per the instructinons we move the main work of labwork to
    // user_isr.
    IFSCLR(0) = (1 << 8);
    timeoutcount++;
    
    if (timeoutcount == 10) {
        timeoutcount = 0;	
        
        time2string( textstring, mytime );
        display_string( 3, textstring );
        display_update();
        tick( &mytime );
    }
}

/* Lab-specific initialization goes here */
void labinit(void)
{ 
	TRISESET = ~0xff;           // Set LED bits to outputs
	TRISDSET = 0xfe0;           // Set Switches and Buttons to inputs
    
    T2CONCLR  = 1 << 15;        // Turn the timer off
    TMR2       = 0;             // Reset timer value
    T2CONSET   = 0x70;          // Set prescaling to 111 -> 1:256 (0111 0000) 
    PR2        = 31250;         // Set period to 80 000 000 / (256 * 10) (1s/10 = 100ms) 
    IFSCLR(0)  = 1 << 8;        // Reset interrupt flag
    T2CONSET   = 1 << 15;       // Set timer 2 to ON 
     
    // From the previous part where we dug through the family data sheet we will 
    // likewise find out bit placements there. But how do we know what to look for?
    // Except for looking at the instructions, we can also find it through the
    // "Section 8: Interrupts" manual. Heading to the "Control Register" chapter, again
    // we can see a number of different registers and register parts. We want to enable 
    // interrupts so that is what we'll look for.

    // IECx mentions interrputs ans is actually an acronym for "Interrupt Enable Control [Register x]".
    // What we want is to enable an interrupt and so the IECx register fits us perfectly.
    // To find what bit we, as mentioned earlier, look through the data sheet but now we know
    // to look on the IECx rows.

    // But you also might remember that above TxIF there was an "TxIE" in IEC0. As you may
    // guess, the IE part is an acronym for "Interrupt Enable". Now we know that we should look for
    // T2IE, as were working on timer 2.
    IECSET(0) = (1 << 8);       // Set interrupt bit

    // Below IECx you can see IPCx, which stands for Interrupt Pirority Control.
    // At first glance this seems to be something unnecessary for the "simple" things we are doing.
    // However, if you jump to its section of that manual, you will see that complete 0 values means
    // disabled, which is not what we want.
    // To make it simple for us, we set both priority and sub-priority to complete 1 values. There
    // are no other interrupts so it does not matter.
    IPCSET(2) = 0x1f;           // Set both prority and sub priority to highest
   
    // We call the enable_interrupt subroutine so that interrupts are enabled on the CPU.
    enable_interrupt();
}

/* This function is called repetitively from the main program */
void labwork(void)
{
    // The instructions has told us to replace the labwork
    // body with this new body.
    prime = nextprime( prime );
    display_string( 0, itoaconv( prime ) );
    display_update();
}
