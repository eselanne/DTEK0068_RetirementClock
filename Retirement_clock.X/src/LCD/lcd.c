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
#include "../CMD/cmd.h"



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

void LCD_set_view(enum LCD_views view, struct tm *timeinfo)
{
    switch (view)
    {
        case CLOCK_VIEW:;
            // Format and copy date to string
            char time_str[100];
            char date_str[100];
            strftime(time_str, sizeof(time_str), "%H:%M:%S", timeinfo);
            strftime(date_str, sizeof(date_str), "%d.%m.%Y", timeinfo);
            // Calculate starting indexes
            uint8_t time_index = ((16 - strlen(time_str)) / 2) + 1;
            uint8_t date_index = ((16 - strlen(date_str)) / 2) + 1;
            // Display on LCD
            LCD_clear();
            LCD_goto(1,time_index);
            LCD_print(time_str);
            LCD_goto(2,date_index);
            LCD_print(date_str);
            break;            
        case COUNTDOWN_VIEW:;
            // Calculate starting index
            int8_t start_index = ((16 - strlen("Time to RET:")) / 2) + 1;
            // Display on LCD
            LCD_clear();
            LCD_goto(1, start_index);
            LCD_print("Time to RET:");
            LCD_goto(2, 8);
            LCD_print("0"); // TODO
            break;
        case UPTIME_VIEW:;
            // TODO
            break;
    }
}

void LCD_update_view()
{
	// TODO instead of CLOCK_VIEW use ACTIVE_VIEW ?
    LCD_set_view(CLOCK_VIEW, DATETIME);
}
