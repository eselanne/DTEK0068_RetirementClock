/*
 * LCD_m4809.c
 *
 * Created: 06-Jan-20 10:01:53
 * Author : David Prentice
 */ 

#define F_CPU   3333333

#include <avr/io.h>
#include <util/delay.h>
#include "lcd.h"
//#include "twswitch.c"  //.kbv unknown file

int main(void)
{
    PORTB.DIRSET = PIN5_bm;
    PORTB.OUTSET = PIN5_bm;
    lcdInit();
    while(1) {
        lcdCmd(0x01);  //blank screen
        _delay_ms(1000); //.kbv needs at least 2ms after cmd(0x01)
        lcdCmd(0x80);  //lcd_gotoxy(0,0)
        lcdString("huutista");
        lcdCmd(0xC0);  //lcd_gotoxy(0,1)
        lcdString("8===D");
        _delay_ms(1000);
    }    
}

