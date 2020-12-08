/*
 * lcd.c
 *
 * Created: 06-Jan-20 10:02:57
 *  Author: David Prentice
 */ 
/*** PIN ASSIGNMENTS ***/
/*
EN - PC0
RS - PC1
D4 - PC4
D5 - PC5
D6 - PC6
D7 - PC7
*/

#define F_CPU   3333333

#include <avr/io.h>
#include <util/delay.h>
#include "lcd.h"

/* INITIALIZATION */
void lcdInit(void)
{
    lcdDir = 0xF3;	                //.kbv LCD port direction as output i.e. PC0/1/4/5/6/7
    _delay_ms(20);					//LCD power initialization time

    lcdCmd(0x02);						//Initializes LCD in 4-bit mode
    lcdCmd(0x28);						//Configures LCD in 2-line 4-bit mode and 5x7 matrix
    lcdCmd(0x0c);						//Display on, cursor off
    lcdCmd(0x06);						//Increment cursor: shift cursor to right
    lcdCmd(0x01);						//Clear display screen
}

/* FOR CLEARING LCD DISPLAY */
void lcdClear()
{
    lcdCmd(0x01);						//Clear display screen
    _delay_ms(2);
    lcdCmd(0x08);						//Clear display without clearing ram content
}

/* LCD COMMAND MODE */
void lcdCmd(uint8_t cmnd)
{
    //Sending upper nibble since 4-bit
    lcdPort = (lcdPort & 0x0F) | (cmnd & 0xF0);
    lcdPort &= ~(1<<RS);					//RS OFF
    lcdPort |= (1<<EN);					//Set bit in EN
    _delay_us(1);
    lcdPort &= ~(1<<EN);					//Clear bit in EN
    _delay_us(200);
    //Sending lower nibble
    lcdPort = (lcdPort &0x0F) | (cmnd <<4);
    lcdPort |= (1<<EN);
    _delay_us(1);
    lcdPort &= ~(1<<EN);
    _delay_ms(2);
}

/* ACCEPTING CHARACTERS IN LCD */
void lcdChar(uint8_t cmnd)
{
    //Sending upper nibble since 4-bit
    lcdPort = (lcdPort & 0x0F) | (cmnd & 0xF0);
    lcdPort |= (1<<RS);					//.kbv RS ON
    lcdPort |= (1<<EN);					//Set bit in EN
    _delay_us(1);
    lcdPort &= ~(1<<EN);					//Clear bit in EN
    _delay_us(200);
    //Sending lower nibble
    lcdPort = (lcdPort &0x0F) | (cmnd <<4);
    lcdPort |= (1<<EN);
    _delay_us(1);
    lcdPort &= ~(1<<EN);
    _delay_ms(2);
}

/* CONVERTS THE CHARACTER INTO STRING */
void lcdString(uint8_t *str)
{
    int i;
    for(i=0; str[i] != 0; i++)
    {
        lcdChar(str[i]);
    }
}
