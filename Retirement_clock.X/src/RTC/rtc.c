/*
 * File:   rtc.c
 * Author: dtek0068
 *
 * Created on 09 December 2020, 00:13
 */

#define F_CPU 3333333

#include <xc.h>
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>
#include <avr/cpufunc.h>
#include "../LCD/lcd.h"
#include "../DATE/date.h"
#include "rtc.h"


volatile int16_t backlight_duration = 5;
volatile int backlight_counter; //t�h�n joku parempi ratkasu :D
int counter = 0;
char str[16]; //stringi, johon tallennetaan int arvo

void RTC_init()
{
    PORTF.DIRCLR = PIN6_bm; //set button as a input
    PORTF.PIN6CTRL = PORT_ISC_FALLING_gc; //configured to trigger an interrupt when state goes low (when button is pressed)
    PORTF.DIRSET = PIN5_bm; //set LED as a output (TESTAUKSEEN)
    
    /* Run in debug: enabled */
    RTC.DBGCTRL = RTC_DBGRUN_bm;
    
    RTC.PITINTCTRL = RTC_PI_bm; /* Periodic Interrupt: enabled */    
    RTC.PITCTRLA = RTC_PERIOD_CYC32768_gc /* RTC Clock Cycles 32768 */
                 | RTC_PITEN_bm; /* Enable: enabled */
    
    sei(); // Enable interrupt
}

//RTC interrupt
ISR(RTC_PIT_vect) 
{
    DATE_incr_one_sec(); // Increments relevant dates by 1 sec
    LCD_update_view();
    
    //testiprinttaus n�ytt��n, konvertointi INT --> String
    //sprintf(str, "%d", counter);
	//LCD_goto(2,3);
	//LCD_print(str);
    
    RTC.PITINTFLAGS = RTC_PITEN_bm;//Clear all interrupt flags
    PORTF.OUTTGL = PIN5_bm; //AVR-Led Toggle ON/OFF (TESTAUKSEEN)
    
    
    if (backlight_counter < backlight_duration && backlight_counter >= 0) {
        backlight_counter++;
    }
    else 
    {
        //PORTF.OUTCLR = PIN2_bm;
    }
    counter++;
}

//button interrupt
ISR(PORTF_PORT_vect) 
{
    PORTF.INTFLAGS = 0xFF;//Clear all interrupt flags
    PORTF.OUTTGL = PIN2_bm;
    backlight_counter = 0;
    
}
