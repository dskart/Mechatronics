/* Name: main.c
 * Author: <Raphael Van Hoffelen>
 */

//include libraries
#include "teensy_general.h"  // includes the resources included in the teensy_general.h file
#include "own_functions.h"

// define constants
#define Hz 1000

//----------------------------------------------GLOBAL VARIABLES--------------------------------------------------
// set global variables
int n = 4;
int ii =0;
int position_sensor[4];
int value = 0;
int side =0;
int calib = 1;

//------------------------------------------------FUNCTIONS/INTERRUPTS----------------------------------------

// set adc interupt
ISR(ADC_vect){

    if (ii == 1) {


        // reads adc and replace element in array
        position_sensor[n-4] = ADC;

        // switch adc channel (0-1) two ochannels
        n += 1;
        if (n > 7){
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
//set functions
void X_Y_joystick(int value, int jj);

// ------------------------------------------------------SETUP---------------------------------------------------------------
int main(void)
{
    //initiates USB
    m_usb_init();
    while(!m_usb_isconnected()); //waits for usb to be connected
    // start timers and set internal clock
    teensy_clockdivide(1);
    set_timer1();
    set_timer3();

    // initializes pins

    //output for pwm value
    set(DDRB,5); // linked with D0
    set(DDRB,6); // linked with D1
    set(DDRB,7); // sliding pot
    set(DDRC,6); // TURRET

    //output for direction of each motor
    set(DDRD,0); // linked with B5
    set(DDRD,1); // linked with B6

    // input

    // starts ADC( read  previous labs to find what that fucntion does)
    set_adc(4,1,0); // F4 ---- X
    set_adc(5,1,0); // F5 ---- Y
    set_adc(6,1,0); // F6 ---- TURRET
    set_adc(7,1,0); // F7 ---- SLIDING POT
    teensy_wait(100);

    //starts convertion
    start_adc(1);

    // enables and start adc interrupt
    set(ADCSRA, ADIE);
    sei();

    // -----------------------------------------------MAIN LOOP -----------------------------------------------------------
    for(;;){

        //set a variable
        int jj =0;

        //cycles through position vector
        for(jj =0; jj <4; jj+=1){

            int value = position_sensor[jj] /10.23  ;// maps value depending on joystick

            // X_Y JOYSTICKS
            if (jj == 0 || jj == 1){ // ADC F4,F5
                X_Y_joystick(value, jj);
                //m_usb_tx_uint(value);
                //m_usb_tx_string("\n");

            // TURET JOYSTICK
            }else if ( jj == 2){ //ADC F6

                if (40<=value && 60>=value){ // velocity of turret = 0
                    value = 0;
                }else if ( value > 60){ // velocity of turret is positive ( counterclockwise)
                    value = 90;
                }else if ( value < 40){ // velocity of turret is negative ( counter clockwise)
                    value = 50;
                }
                set_Hz_C6(Hz, value);


            //SLIDING POT
            }else { //ADC F7
                if (value > 97){ // make sure it never sends a PWM of 100
                    value = 98;
                }
                set_Hz_B7(Hz, value);

            }

        }
    }

    return 0;   /* never reached */
}

//-------------------------------------------------FUNCTION DEF-------------------------------------------------

void X_Y_joystick(int value, int jj){
    // joystick is at the center
    if (value == 50 || value == 51){
        value = 0; // map joystick abs value
        side = 1; // set side as positive
        if (jj ==0){
            calib = 1;
        }
    }
    // joystick is positive
    else if (value > 51){
        value = (value - 50) * 2; // map joystick abs value
        side = 1; // set side as positive
        calib = 0;

    //joystick is negative
    }else if (value < 50){
        value = abs((value -50 ) * 2); // map joystick abs value
        side = 0; //set side as postive
        calib = 0;

    }

    // checks if postion is negative or positive
    if (side == 1){

        // makes sure it is never 100 (ES1 can t detect 100%duty cycle)
        if (value == 100){
            value = 98;
        }

        // sets right pins deppending on which joystick we are reading (value of jj corresponds to each joystick)
        if (jj == 0){
            set_Hz_B5(Hz, value);

            //calib here is used to make sure we output the right value for the sign pin ( joystick is upside down)
            if(calib ==0){
             set(PORTD,0);
         }else{
             clear(PORTD,0);
         }

        }else {
            set_Hz_B6(Hz, value);
            clear(PORTD,1);
        }

    }else if (side ==0){

        // makes sure it is never 100 (ES1 can t detect 100%duty cycle)
        if (value == 100){
            value = 98;
        }

          // sets right pins deppending on which joystick we are reading (value of jj corresponds to each joystick)
        if (jj == 0){
            set_Hz_B5(Hz, value);
            clear(PORTD,0);
        }else {
            set_Hz_B6(Hz, value);
            set(PORTD,1);
        }

    }

}
