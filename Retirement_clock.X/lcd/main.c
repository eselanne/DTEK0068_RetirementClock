/*
 * File:   main.c
 * Author: dtek0068
 *
 * Created on 08 December 2020, 22:04
 */

#define F_CPU 3333333

#include <avr/io.h>
#include <util/delay.h>
#include <util/delay.h>
#include "lcd.h"

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
    
    PORTB.DIRSET = PIN5_bm;
    PORTB.OUTSET = PIN5_bm;
 
	while(1)
    {
         
    } 
}