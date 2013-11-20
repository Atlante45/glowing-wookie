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
		while(!is_ready_to_send());
		UDR = msg[i];
	}
}

int recv_msg(char * buffer, int size)
{
	int i = 0;
	while(is_ready_to_read() && i < size)
	{
		buffer[i] = UDR;
		i++;
	}
	return i;
}

int is_ready_to_send()
{
	return (UCSRA & (1 << UDRE));
}

int is_ready_to_read()
{
	return (UCSRA & (1 << RXC)); 
}
