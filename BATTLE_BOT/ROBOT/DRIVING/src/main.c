/* Name: main.c
 * Author: <Raphael Van Hoffelen>
 */

//include libraries
#include "teensy_general.h"  // includes the resources included in the teensy_general.h file
#include "own_functions.h"

//define constant
#define cooldown 500

// ------------------------------------------------------------------------------------GLOBAL VARIABLES----------------------------------------------------------

int n = 4;
int ii =0;
int position_sensor[3];
int value = 0;

//---------------------------------------------------------------------------------INTERRUPT--------------------------------------------------------
// set adc interupt
ISR(ADC_vect){

    if (ii == 1) {

        // reads adc and replace element in array
        position_sensor[n-4] = ADC;

        // switch adc channel (0-1) two ochannels
        n += 1;
        if (n > 6){
            n = 4;
        }

        // updates ADC channel
        set_adc(n,1,1);
        //teensy_wait(10);
        ii = 0;

    }else{

         //first read might be from old channel, makes sure to wait second read after switching channel
        ii = 1;

    }
}

//-----------------------------------------------------------------------------SETUP--------------------------------------------------------
int main(void)
{
    //initiates USB
    //m_usb_init();
    //while(!m_usb_isconnected()); //waits for usb to be connected

    // start timers and set internal clock
    teensy_clockdivide(1);
    set_timer1();

    // initializes pins
    set(DDRD,0); // linked with D0

    //ADD INPUT PINS
    clear(DDRD,1);
    clear(DDRD,2);

    set(PORTD,1);
    set(PORTD,2);

    // starts ADC F4 and F5 ( read  previous labs to find what that fucntion does)
    set_adc(4,1,0);    // ADC4 = F4
    set_adc(5,1,0);    // ADC5 = F5
    set_adc(6,1,0);    // ADC6 = F6


    teensy_wait(100);
    //starts convertion
    start_adc(1);

    // enables and start adc interrupt
    set(ADCSRA, ADIE);
    sei();

//---------------------------------------------------------------------------------------MAIN LOOP--------------------------------------------
    for(;;){

        //set a variable
        int jj =0;

        //check in switches attached to the pin are activated
        if (check(PIND, 2) == 0){

            //Turn on "HIT" LED
            set(PORTD, 0);
            teensy_wait(cooldown);
            clear(PORTD, 0);

        }else if (check(PIND,1) == 0){

            //Turn on "HIT" LED
            set(PORTD, 0);
            teensy_wait(cooldown);
            clear(PORTD, 0);
        }


        //cycles through position vector
        for(jj =0; jj <3; jj+=1){
            int value = position_sensor[jj] ;// change value depending on joystick

            // if some pressure is found on the force sensors
            if (value >100){

                //Turn on "HIT" LED
                set(PORTD, 0);
                teensy_wait(cooldown);
                clear(PORTD, 0);
            }

        }


    }

    return 0;   /* never reached */
}
