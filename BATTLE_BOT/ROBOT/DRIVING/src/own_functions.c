#include "teensy_general.h"
#include "own_functions.h"




void set_adc(int adc, int free_run,int adc_switch) {

    // choose adc number to initialize,
    //choose if you want free_running mode (=1)
    // if adc_switch set to 1, it means you want to switch which adc channel you are reading
    // must be done after you initialized all the adc ports you wanted
    //if set adc_switch to 0 -> initialize adc port and select last set adc port as active channel

    // adc # corresponding to teensy pin
    // ADC0 = F0
    // ADC1 = F1
    // ADC4 = F4
    // ADC5 = F5
    // ADC6 = F6
    // ADC7 = F7
    // ADC8 = D4
    // ADC9 = D6
    // ADC10 = D7
    // ADC11 = B4
    // ADC12 = B5
    // ADC13 = B6

    if (adc_switch == 0 ){

        set(ADMUX,REFS0); // sets voltage reference to Vcc
        set(ADCSRA, ADPS2); //sets adc clock to 125kHz

        if (adc<8) {
            set(DIDR0,adc); // ENABLE THE RIGHT PIN (f1)
        } else if (adc>8){
            set(DIDR2,adc);
        }

        if (free_run == 1){
            set(ADCSRA,ADATE); // set trigger in "free running mode"
        }
    }

        switch(adc) {
        case 0 :
            clear(ADMUX,MUX0);
            clear(ADMUX,MUX1);
            clear(ADMUX,MUX2);
            clear(ADCSRB,MUX5);
            break;
        case 1 :
            set(ADMUX,MUX0);
            clear(ADMUX,MUX1);
            clear(ADMUX,MUX2);
            clear(ADCSRB,MUX5);
            break;

        case 4 :
            clear(ADMUX,MUX0);
            clear(ADMUX,MUX1);
            set(ADMUX,MUX2);
            clear(ADCSRB,MUX5);
            break;

        case 5 :
            set(ADMUX,MUX0);
            clear(ADMUX,MUX1);
            set(ADMUX,MUX2);
            clear(ADCSRB,MUX5);
            break;

        case 6 :
            clear(ADMUX,MUX0);
            set(ADMUX,MUX1);
            set(ADMUX,MUX2);
            clear(ADCSRB,MUX5);
            break;

        case 7 :
            clear(ADMUX,MUX0);
            set(ADMUX,MUX1);
            set(ADMUX,MUX2);
            set(ADCSRB,MUX5);
            break;

        case 8 :
            clear(ADMUX,MUX0);
            clear(ADMUX,MUX1);
            clear(ADMUX,MUX2);
            set(ADCSRB,MUX5);
            break;

        case 9 :
            set(ADMUX,MUX0);
            clear(ADMUX,MUX1);
            clear(ADMUX,MUX2);
            set(ADCSRB,MUX5);
            break;

        case 10 :
            clear(ADMUX,MUX0);
            set(ADMUX,MUX1);
            clear(ADMUX,MUX2);
            set(ADCSRB,MUX5);
            break;

        case 11 :
            set(ADMUX,MUX0);
            set(ADMUX,MUX1);
            clear(ADMUX,MUX2);
            set(ADCSRB,MUX5);
            break;

        case 12 :
            clear(ADMUX,MUX0);
            clear(ADMUX,MUX1);
            set(ADMUX,MUX2);
            set(ADCSRB,MUX5);
            break;

        case 13 :
            set(ADMUX,MUX0);
            clear(ADMUX,MUX1);
            set(ADMUX,MUX2);
            set(ADCSRB,MUX5);
            break;

    }
    return;
}

int start_adc(state){
    //starts adc on which ever channel you selected
    // state = 1 if you want to enable conversion
    // state = 0 if you want to stop conversion
    // even if you are in free running mode, you have to run the function once with state =1
    // to start the conversion

            if (state ==1){
            //starts convertions
            set(ADCSRA, ADEN);
            set(ADCSRA, ADSC);
            return 1;

            }else{
                clear(ADCSRA, ADEN); // stop conversion to change channel
                return 0;
            }
}
