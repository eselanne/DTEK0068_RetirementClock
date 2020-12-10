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
#include "rtc.h"


int16_t backlight_duration = 5;
int backlight_counter; //t�h�n joku parempi ratkasu :D
int counter = 0;
char str[16]; //stringi, johon tallennetaan int arvo

void RTC_init()
{
    PORTF.DIRCLR = PIN6_bm; //set button as a input
    PORTF.PIN6CTRL = PORT_ISC_FALLING_gc; //configured to trigger an interrupt when state goes low (when button is pressed)
    PORTF.DIRSET = PIN5_bm; //set LED as a output (TESTAUKSEEN)
    
    uint8_t temp;
    
    /* Initialize 32.768kHz Oscillator: */
    /* Disable oscillator: */
    temp = CLKCTRL.XOSC32KCTRLA;
    temp &= ~CLKCTRL_ENABLE_bm;
    /* Writing to protected register */
    ccp_write_io((void*)&CLKCTRL.XOSC32KCTRLA, temp);
    
    while(CLKCTRL.MCLKSTATUS & CLKCTRL_XOSC32KS_bm)
    {
        ; /* Wait until XOSC32KS becomes 0 */
    }
    
    /* SEL = 0 (Use External Crystal): */
    temp = CLKCTRL.XOSC32KCTRLA;
    temp &= ~CLKCTRL_SEL_bm;
    /* Writing to protected register */
    ccp_write_io((void*)&CLKCTRL.XOSC32KCTRLA, temp);
    
    /* Enable oscillator: */
    temp = CLKCTRL.XOSC32KCTRLA;
    temp |= CLKCTRL_ENABLE_bm;
    /* Writing to protected register */
    ccp_write_io((void*)&CLKCTRL.XOSC32KCTRLA, temp);
    
    /* Initialize RTC: */
    while (RTC.STATUS > 0)
    {
        ; /* Wait for all register to be synchronized */
    }

    /* 32.768kHz External Crystal Oscillator (XOSC32K) */
    RTC.CLKSEL = RTC_CLKSEL_TOSC32K_gc;

    /* Run in debug: enabled */
    RTC.DBGCTRL = RTC_DBGRUN_bm;
    
    RTC.PITINTCTRL = RTC_PI_bm; /* Periodic Interrupt: enabled */
    
    RTC.PITCTRLA = RTC_PERIOD_CYC32768_gc; /* RTC Clock Cycles 32768 */
             //    | RTC_PITEN_bm; /* Enable: enabled */
    
    sei(); // Enable interrupt
    
    RTC.PITCTRLA |= RTC_PITEN_bm; // Start RTC
}

//RTC interrupt
ISR(RTC_PIT_vect) 
{
    //testiprinttaus n�ytt��n, konvertointi INT --> String
    sprintf(str, "%d", counter);
	LCD_goto(2,3);
	LCD_print(str);
    
    RTC.PITINTFLAGS = RTC_PITEN_bm;//Clear all interrupt flags
    PORTF.OUTTGL = PIN5_bm; //AVR-Led Toggle ON/OFF (TESTAUKSEEN)
    
    
    if (backlight_counter < backlight_duration && backlight_counter >= 0) {
        backlight_counter++;
    }
    else 
    {
        PORTF.OUTCLR = PIN2_bm;
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
