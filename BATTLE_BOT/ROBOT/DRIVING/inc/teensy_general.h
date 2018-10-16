// -----------------------------------------------------------------------------
// Teensy general macros adapted from m_general.h by J. Fiene
// version: 2
// date: September 6, 2017
// author: adapted by P. Stegall / Mark Yim
// -----------------------------------------------------------------------------
#ifndef teensy_general__
#define teensy_general__

// -----------------------------------------------------------------------------
// Useful pre-compile constants
// -----------------------------------------------------------------------------

#define TRUE    1
#define FALSE   0

#define OFF     0
#define ON      1
#define TOGGLE      2

// -----------------------------------------------------------------------------
// General AVR libraries:
// -----------------------------------------------------------------------------

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdint.h>
#include <stdbool.h>
#include <avr/pgmspace.h>
#include <math.h>
#include <t_usb.h>

// -----------------------------------------------------------------------------
// Bit manipulation and validation:
// -----------------------------------------------------------------------------

#define set(reg,bit)        (reg) |= (1<<(bit))
#define clear(reg,bit)      (reg) &= ~(1<<(bit))
#define toggle(reg,bit)     (reg) ^= (1<<(bit))
#define check(reg,bit)                (bool)(reg & (1 <<(bit)))
// -----------------------------------------------------------------------------
// Disable JTAG to access F4-F7:
// -----------------------------------------------------------------------------

#define teensy_disableJTAG()        MCUCR = (1 << JTD); MCUCR = (1 << JTD)
// Setting the JTD bit in MCUCR twice within four clock cycles will allow user
// access to F4-F7 as normal port pins. Note that using |= is too slow for this
// operation to work correctly, so we are setting the entire register
// (fortunately, all other bits in MCUCR are 0 anyway).


// -----------------------------------------------------------------------------
// Set the system clock:
// -----------------------------------------------------------------------------

#define teensy_clockdivide(val) CLKPR = (1<<CLKPCE); CLKPR=val
// "val" must be an integer from 0 to 8
// this will divide the 16MHz system clock by 2^val:
// 0 = 16 MHz
// 1 = 8 MHz
// 2 = 4 MHz
// 3 = 2 MHz
// 4 = 1 MHz
// 5 = 500 kHz
// 6 = 250 kHz
// 7 = 125 kHz
// 8 = 62.5 kHz


// -----------------------------------------------------------------------------
// Wait for a specified number of milliseconds:
// -----------------------------------------------------------------------------

#define teensy_wait(val)    _delay_ms(val)
// "val" must be an integer from 1 to 65535
// this function assumes a 16MHz clock


// -----------------------------------------------------------------------------
// Change the state of the on-board LED:
// (included here for educational purposes)
// -----------------------------------------------------------------------------

#define teensy_led(val)     set(DDRD,7); if(val==ON){set(PORTD,7);}else if(val==OFF){clear(PORTD,7);}else if(val==TOGGLE){toggle(PORTD,7);}
// "val" must be either OFF, ON, or TOGGLE, as defined above


#define set_Hz(hz, pwm)                     OCR3A = ((2000000/(hz * 2)) * (pwm/100.00)); ICR3= 2000000/(hz * 2);

#define set_Hz_B5(hz, pwm)                     OCR1A = ((2000000/(hz * 2)) * (pwm/100.00)); ICR1= 2000000/(hz * 2);
#define set_Hz_B6(hz, pwm)                     OCR1B = ((2000000/(hz * 2)) * (pwm/100.00)); ICR1= 2000000/(hz * 2);

#define set_timer3()            set(PORTD,7);\
                                                 set(TCCR3A,COM3A1); \
                                                 set(TCCR3B,WGM33); \
                                                 set(TCCR3B,WGM32); \
                                                 set(TCCR3A,WGM31); \
                                                 set(TCCR3B,CS31);

#define set_timer1()              set(PORTB,5);set(PORTB,6);\
                                                 set(TCCR1A, COM1A1);\
                                                 set(TCCR1A, COM1B1);\
                                                 set(TCCR1B,WGM33); \
                                                 set(TCCR1B,WGM32); \
                                                 set(TCCR1A,WGM31); \
                                                 set(TCCR1B,CS11);
#endif
