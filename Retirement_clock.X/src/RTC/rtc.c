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
#include "../USART/usart.h"
#include "rtc.h"

volatile uint16_t backlight_duration = 5;
volatile uint16_t backlight_counter = 0;

void RTC_init()
{
    PORTF.DIRCLR = PIN6_bm; // Set button as a input (internal button)
    PORTE.DIRCLR = PIN0_bm; // Set button as a input (external button)
    
    // Configured to trigger an interrupt
    // when state goes low (when button is pressed)
    PORTF.PIN6CTRL = PORT_ISC_FALLING_gc;
    PORTE.PIN0CTRL = PORT_ISC_FALLING_gc;
    
    PORTE.PIN0CTRL |= PORT_PULLUPEN_bm; // Enable pull up resistor
    
    PORTF.DIRSET = PIN5_bm; // Set LED as a output (TESTAUKSEEN)
    PORTC.DIRSET = PIN4_bm; // Set BUZZER as a output    
    
    /* Run in debug: enabled */
    RTC.DBGCTRL = RTC_DBGRUN_bm;
    
    RTC.PITINTCTRL = RTC_PI_bm; /* Periodic Interrupt: enabled */    
    RTC.PITCTRLA = RTC_PERIOD_CYC32768_gc /* RTC Clock Cycles 32768 */
                 | RTC_PITEN_bm; /* Enable: enabled */
    
    sei(); // Enable interrupt
}

// RTC interrupt
ISR(RTC_PIT_vect) 
{
    DATE_incr_one_sec(); // Increments relevant dates by 1 sec
    LCD_update_view(); // Prints active view to LCD
    backlight_counter++;
    
    if(backlight_counter >= backlight_duration)
    {
        backlight_counter = 0;
        PORTF.OUTCLR = PIN2_bm;
    }
    
    RTC.PITINTFLAGS = RTC_PITEN_bm; // Clear all interrupt flags
    PORTF.OUTTGL = PIN5_bm; // AVR-Led Toggle ON/OFF (TESTAUKSEEN)
}

// PORTF button interrupt (backlight on)
ISR(PORTF_PORT_vect)
{
    PORTF.INTFLAGS = 0xFF; // Clear all interrupt flags
    PORTF.OUTSET = PIN2_bm;
    backlight_counter = 0;    
}

// PORTE button interrupt (rotate views)
ISR(PORTE_PORT_vect)
{
    PORTE.INTFLAGS = 0xFF; // Clear all interrupt flags
    LCD_rotate_view();
    // Stop buzzer
    PORTC.OUTCLR = PIN4_bm;
    
}