#include <avr/io.h>
#include <util/delay.h>
#include "communication.h"

int main()
{
	char * s = "Hello World!\n";	
	init_com();
	while(1)
	{
		send_msg(s,13);
		_delay_ms(1000.);
	}
}
