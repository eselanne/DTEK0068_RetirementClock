/*
 * Sponsored by: 
 * https://github.com/MicrochipTech/TB3216_Getting_Started_with_USART
 */

#include <avr/io.h>
#include <stdio.h>
#include <string.h>
#include "../USART/usart.h"
#include "../LCD/lcd.h"

void USART0_init(void)
{
    PORTA.DIR &= ~PIN1_bm;
    PORTA.DIR |= PIN0_bm;
    
    USART0.BAUD = (uint16_t)USART0_BAUD_RATE(9600);

    USART0.CTRLB |= USART_RXEN_bm | USART_TXEN_bm;
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