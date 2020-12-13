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
#include "DATE/date.h"
#include "RTC/rtc.h"

int exec(char *cmd);

int main(void) 
{     
    // Init stuff
    LCD_init();	
    DATE_init();
    LCD_update_view();
    USART0_init();
    RTC_init();

    char command[MAX_COMMAND_LEN];
    uint8_t index = 0;
    char c;
    uint8_t exit_code = 0;    
   
    while (1)
    {
        c = USART0_readChar(); // Read serial user interface char by char
        switch (c)
        {
        	// TODO handle arrows here ?
            case 127:
                // Backspace
                command[--index] = '\0';
                break;
            case '\r':
                // Carriage return
                command[index] = '\0';
                index = 0;
                USART0_sendString("\r\n");
                exit_code = exec(command);
                break;
            default:
                command[index++] = c;
                if(index > MAX_COMMAND_LEN)
                {
                    index = 0;
                }
        } 
        USART0_sendChar(c); // Show the char        
        if(exit_code != 0)
        {
            USART0_sendString("Invalid command\r\n");
            exit_code = 0;
        }
    }
}

int exec(char *cmd)
{    
    // Parse command
    char args[16][32];
    uint8_t i = 0;
    uint8_t j = 0;
    uint8_t ctr = 0;
    for(i = 0; i <= (strlen(cmd)); i++)
    {
        // if space or NULL found, assign NULL into args[ctr]
        if((cmd[i] == ' ') || (cmd[i] == '\0'))
        {
            args[ctr][j] = '\0';
            ctr++;  //for next word
            j = 0;  //for next word, init index to 0
        }
        else
        {
            args[ctr][j] = cmd[i];
            j++;
        }
    }

    // Handle date command
    if((strcmp(args[1], "DATETIME") == 0) || (strcmp(args[1], "BIRTHDAY") == 0))
    {
        if(DATE_handle_date_cmd(args[0], args[1], args[2], args[3]) == 0)
        {
            return 0;
        }
    }
    
    // Handle backlight command
    if(strcmp(args[1], "BACKLIGHT") == 0)
    {
        if(strcmp(args[0], "GET") == 0)
        {
            char msg_str[100];
            snprintf(msg_str, sizeof(msg_str),
                    "LCD backlight duration: %d seconds\r\n",
                    backlight_duration);
            USART0_sendString(msg_str);
            return 0;
        }
        if(strcmp(args[0], "SET") == 0)
        {
            int16_t duration = atoi(args[2]);
            backlight_duration = duration > 0 ? duration : 5;
            return 0;
        }
    }
    
    return 1;
}