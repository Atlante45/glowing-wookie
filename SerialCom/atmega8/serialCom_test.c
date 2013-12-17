#include <avr/io.h>
#include <util/delay.h>
#include "communication.h"

#define BUFFER_SIZE 1

int main()
{
	char * s = "Hello World!\n";	
	char buffer[BUFFER_SIZE];
	int size;
	init_com();
	while(1)
	{
      		size = recv_msg(buffer,BUFFER_SIZE);				
		send_msg(buffer,size);
		//send_msg(s,13);
		//_delay_ms(1000.);
	}
}
