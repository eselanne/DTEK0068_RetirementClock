/*
 * Inspiration from :D
 * https://alselectro.wordpress.com/2017/08/13/
avr-self-learning-kit-interfacing-16-x-2-lcd-in-8-bit-mode/
 */

#define F_CPU 3333333

#include <avr/io.h>
#include <util/delay.h>
#include "../LCD/lcd.h"

void LCD_init()
{
    _delay_ms(10);
    LCD_send_command(0x38);
    LCD_send_command(0x0E);
    LCD_send_command(0x01);
    _delay_ms(10);
    LCD_send_command(0x06);
    LCD_send_command(0x0C);
}

void LCD_send_command(unsigned char cmnd)
{
    DATA_PORT = cmnd;
    CNTRL_PORT &= ~(1<< RW_PIN);
    CNTRL_PORT &= ~(1<< RS_PIN);
    
    CNTRL_PORT |= (1<< ENABLE_PIN);
    _delay_us(2);
    CNTRL_PORT &= ~(1<< ENABLE_PIN);
    _delay_us(100);
}

void LCD_send_data(unsigned char data)
{
    DATA_PORT = data;
    CNTRL_PORT &= ~(1<< RW_PIN);
    CNTRL_PORT |= (1<< RS_PIN);

    CNTRL_PORT |= (1<< ENABLE_PIN);
    _delay_us(2);
    CNTRL_PORT &= ~(1<< ENABLE_PIN);
    _delay_us(100);
}

void LCD_goto(unsigned char y, unsigned char x)
{
    unsigned char firstAddress[] = {0x80,0xC0,0x94,0xD4};
    LCD_send_command(firstAddress[y-1] + x-1);
    _delay_ms(10);
}

void LCD_print(char *string)
{
    while(*string > 0)
    {
        LCD_send_data(*string++);
    }
}

void LCD_blink()
{
    LCD_send_command(0x08);
    _delay_ms(250);
    LCD_send_command(0x0C);
    _delay_ms(250);
}

void LCD_clear(void)
{
    LCD_send_command(0x01);
    _delay_ms(100);
}
