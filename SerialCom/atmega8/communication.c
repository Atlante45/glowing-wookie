#include "communication.h"

#include <avr/io.h>

int send_msg(char * msg, int size)
{
	int i;
	for(i=0; i<size; i++)
	{
		UDR = msg[i];
		while(!(USR&(1<<TXC));
	}
}
