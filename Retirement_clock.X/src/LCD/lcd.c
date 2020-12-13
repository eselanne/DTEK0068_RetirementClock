/*
 * Inspiration from :D
 * https://alselectro.wordpress.com/2017/08/13/
avr-self-learning-kit-interfacing-16-x-2-lcd-in-8-bit-mode/
 */

#define F_CPU 3333333

#include <avr/io.h>
#include <util/delay.h>
#include <time.h>
#include <string.h>
#include "../LCD/lcd.h"
#include "../DATE/date.h"



void LCD_init()
{       
    CNTRL_DDR = 0xFF; // Set out
    CNTRL_PORT = 0x00; // Set in
    DATA_DDR = 0xFF; // Set out
    DATA_PORT = 0x00; // Set in
    
    _delay_ms(10);
    LCD_send_command(0x38); // Define 16 x 2 LCD
    LCD_send_command(0x0C); // Display on, cursor off
    LCD_send_command(0x01); // Clear LCD
    _delay_ms(10);
    LCD_send_command(0x06); // Increment cursor
    
    // Possible values: CLOCK_VIEW, RETIREMENT_VIEW and UPTIME_VIEW
    LCD_view = UPTIME_VIEW;
    
    // Init backlight
    PORTF.DIRSET = PIN2_bm;
    //PORTF.OUTCLR = PIN2_bm; // Off
    PORTF.OUTSET = PIN2_bm; // On

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
    //_delay_ms(2);
}

void LCD_print(char *string)
{
    while(*string > 0)
    {
        LCD_send_data(*string++);
    }
}

void LCD_clear(void)
{
    LCD_send_command(0x01);
    _delay_ms(2);
}

void LCD_update_view()
{
    // TODO
    char row1_str[1000];
    char row2_str[1000];
    switch (LCD_view)
    {
        case CLOCK_VIEW:
            // Format and copy date to string            
            strftime(row1_str, sizeof(row1_str), "%H:%M:%S", (struct tm*) DATETIME);
            strftime(row2_str, sizeof(row2_str), "%d.%m.%Y", (struct tm*) DATETIME);                      
            break;            
        case COUNTDOWN_VIEW:
            strcpy(row1_str, "Retirement in");
            DATE_get_countdown(row2_str);
            break;
        case UPTIME_VIEW:
            strcpy(row1_str, "Uptime");
            DATE_get_uptime(row2_str);
            break;
    }
    // Calculate starting indexes
    uint8_t row1_index = ((16 - strlen(row1_str)) / 2) + 1;
    uint8_t row2_index = ((16 - strlen(row2_str)) / 2) + 1;  
    // Display on LCD
    LCD_clear();
    LCD_goto(1, row1_index);
    LCD_print(row1_str);
    LCD_goto(2, row2_index);
    LCD_print(row2_str);
}
