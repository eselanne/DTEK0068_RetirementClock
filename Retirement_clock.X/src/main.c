/*
 * File:   main.c
 * Author: dtek0068
 *
 * Created on 08 December 2020, 22:04
 */

#define F_CPU 3333333

#include <avr/io.h>
#include <stdio.h>
#include <string.h>
#include <util/delay.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>
#include <avr/cpufunc.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include "LCD/lcd.h"
#include "USART/usart.h"
#include "CMD/cmd.h"
#include "RTC/rtc.h"


int main(void) 
{     
    // Init stuff
    LCD_init();	
    RTC_init();
    CMD_init();
    LCD_set_view(CLOCK_VIEW, DATETIME); // Set some default time
    USART0_init();

    char command[MAX_COMMAND_LEN];
    uint8_t index = 0;
    char c;    
   
    while (1)
    {
        LCD_set_view(CLOCK_VIEW, DATETIME);

        c = USART0_readChar(); // Read serial user interface char by char
        USART0_sendChar(c); // Show the char
        if(c != '\n' && c != '\r') // Check if enter
        {
            command[index++] = c;
            if(index > MAX_COMMAND_LEN)
            {
                index = 0;
            }
        }        
        if(c == '\r')
        {
            command[index] = '\0';
            index = 0;
            USART0_sendChar('\n');
            CMD_exec(command);
        }
    }
}