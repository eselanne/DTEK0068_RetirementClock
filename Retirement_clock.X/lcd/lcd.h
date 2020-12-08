/*
 * lcd.h
 *
 * Created: 06-Jan-20 10:03:28
 *  Author: David Prentice
 */ 

#define F_CPU   3333333


#ifndef LCD_H_
#define LCD_H_


#include <avr/io.h>
#include <util/delay.h>

//.kbv edit defines to suit your wiring.  Note you MUST connect RW pin to GND
//  RS  RW  EN  D4  D5  D6  D7
// PC1 GND PC0 PC4 PC5 PC6 PC7

#define lcdPort PORTC_OUT                   //.kbv Data Output value of PORTC
#define lcdDir PORTC_DIR                    //.kbv Setting pins as I/O
#define EN 0            //.kbv ENABLE (EN) pin to PORTC PIN0
#define RS 1            //.kbv REGISTER SELECT (RS) pin to PORTC PIN1

void lcdInit(void);
void lcdClear();
void lcdCmd(uint8_t cmnd);
void lcdChar(uint8_t cmnd);
void lcdString(uint8_t *str);


#endif /* LCD_H_ */