/*
 * Sponsored by: 
 * https://github.com/MicrochipTech/TB3216_Getting_Started_with_USART
 */

#include <avr/io.h>
#include <stdio.h>
#include <string.h>
#include "../USART/usart.h"
#include "../LCD/lcd.h"

static FILE USART_stream = FDEV_SETUP_STREAM(USART0_printChar, NULL, _FDEV_SETUP_WRITE);

void USART0_init(void)
{
    PORTA.DIR &= ~PIN1_bm;
    PORTA.DIR |= PIN0_bm;
    
    USART0.BAUD = (uint16_t)USART0_BAUD_RATE(9600);

    USART0.CTRLB |= USART_RXEN_bm | USART_TXEN_bm;
    
    //stdout = &USART_stream;    
    //printf("Ready.\n"); not working :(
    
    USART0_sendString("Ready.\r\n");
}

void USART0_sendChar(char c)
{
    while (!(USART0.STATUS & USART_DREIF_bm))
    {
        ;    
    }
    USART0.TXDATAL = c;
}

static int USART0_printChar(char c, FILE *stream)
{
    USART0_sendChar(c);
    return 0;
}

void USART0_sendString(char *str)
{
    for(size_t i = 0; i < strlen(str); i++)
    {
        USART0_sendChar(str[i]);
    }
}

char USART0_readChar(void)
{
    while (!(USART0.STATUS & USART_RXCIF_bm))
    {
        ;
    }
    return USART0.RXDATAL;
}