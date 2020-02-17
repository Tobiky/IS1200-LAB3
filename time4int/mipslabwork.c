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

int prime = 1234567;

int timeoutcount = 0;
int mytime = 0x5957;

char textstring[] = "text, more text, and even more text!";

/* Interrupt Service Routine */
void user_isr(void)
{
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
     
    IECSET(0) = (1 << 8);       // Set interrupt bit
    IPCSET(2) = 0x1f;           // Set both prority and sub priority to highest
    
    enable_interrupt();
}

/* This function is called repetitively from the main program */
void labwork(void)
{
    prime = nextprime( prime );
    display_string( 0, itoaconv( prime ) );
    display_update();
}

//QUESTIONS
//The third and second didgit changes. Because we have an if for if any button is pushed
//TRISE = the value, TRISESET = set the value (index + 1), TRISECLR = clear the value (index + 1)
//v0
//?????
