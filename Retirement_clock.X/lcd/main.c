/*
 * File:   main.c
 * Author: dtek0068
 *
 * Created on 08 December 2020, 22:04
 */

#define F_CPU 3333333

#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>
#include <avr/cpufunc.h>
#include "lcd.h"
#include "rtc.h"



int backlight_counter; //tähän joku parempi ratkasu :D
int backlight_duration = 5; //default value is 5

int main(void) {   
    
    CNTRL_DDR = 0xFF;
	CNTRL_PORT = 0x00;
	DATA_DDR = 0xFF;
	DATA_PORT = 0x00;
	
	LCD_clear();
    LCD_init();
	LCD_goto(1,2);
	LCD_print("moloclan.fi");
	LCD_goto(2,3);
	LCD_print("16x2 LCD 420");
    
    PORTB.DIRSET = PIN5_bm; //set PB5 as a output
    PORTB.OUTCLR = PIN5_bm; //turn off backlight
    PORTF.DIRCLR = PIN6_bm; //set button as a input
    PORTF.PIN6CTRL = PORT_ISC_FALLING_gc; //configured to trigger an interrupt when state goes low (when button is pressed)
    
    set_sleep_mode(SLPCTRL_SMODE_IDLE_gc); //set IDLE as a sleep mode
    
    PORTF.DIRSET = PIN5_bm; //set LED as a output (TESTAUKSEEN)
    
    RTC_init(); 
    
    sei();
 
	while(1)
    {
        RTC.PITCTRLA |= RTC_PITEN_bm; //start RTC
        sleep_mode();
         
    } 
}

//RTC interrupt
ISR(RTC_PIT_vect) 
{
    RTC.PITINTFLAGS = RTC_PITEN_bm;//Clear all interrupt flags
    PORTF.OUTTGL = PIN5_bm; //AVR-Led Toggle ON/OFF (TESTAUKSEEN)
    
    if (backlight_counter < backlight_duration && backlight_counter >= 0) {
        backlight_counter++;
    }
    else 
    {
        PORTB.OUTCLR = PIN5_bm;
    }
}

//button interrupt
ISR(PORTF_PORT_vect) 
{
    PORTF.INTFLAGS = 0xFF;//Clear all interrupt flags
    PORTB.OUTSET = PIN5_bm;
    backlight_counter = 0;
    
}