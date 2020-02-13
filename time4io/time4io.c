#include <stdint.h>
#include <pic32mx.h>
#include "mipslab.h"

int getsw(void) {
	// According to the instructions, the switch states begin at bit 8
    // and end at bit 11. To use these bits more easily, they should be
    // in the least significant part. 

    // That is because it will be easier to perform operations on them
    // if they are. For example, masking. Instead of needing to shift
    // values up or have a large constant like 0x00001000, we can just
    // mask with a value like 0x1 or 0x2 to get what we need.

    // To get the first bit, bit 8, down to the least significant bit (lsb)
    // we need it to go down as many bits, that is we shift it right 
    // 8 times.

    // We then mask the shifted value so we only get switch states, and return
    // the total result to the user of this function.
    return (PORTD >> 8) & 0xf;
}

int getbtns(void) {
    // Same as in getsw(void) above but instead the button states lie on
    // bits 5-7. (0x7 = 0111) 
	return (PORTD >> 5) & 0x7;
}
