#include "communication.h"
#include <avr/io.h>

#define USART_BAUDRATE 9600 
#define BAUD_PRESCALE (((F_CPU / (USART_BAUDRATE * 16UL))) - 1) 

void init_com()
{
	UCSRB = (1 << RXEN) | (1 << TXEN);
	UCSRC = (1 << URSEL) | (1 << UCSZ0) | (1 << UCSZ1);
	UBRRH = (BAUD_PRESCALE >> 8);
	UBRRL = BAUD_PRESCALE;
}

void send_msg(char * msg, int size)
{
	int i;
	for(i=0; i<size; i++)
	{
		UDR = msg[i];
		while( !(UCSRA & (1 << UDRE)));
	}
}

int recv_msg(char * buffer, int size)
{
	int i = 0;
	while ((UCSRA & (1 << RXC)) == 0) {};
	buffer[i] = UDR;
}
