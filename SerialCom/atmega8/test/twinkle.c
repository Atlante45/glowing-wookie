#include <avr/io.h>
#include <util/delay.h>

int main()
{
	int i = 0;
	DDRB = 0x00;
	while(1)
	{
		PORTB = 0b00100000;
		_delay_ms(100*(1+(i%9)));
		PORTB = 0b00000000;
		_delay_ms(100*(1+(i%9)));
		i++;
	}
	return 1;
}
