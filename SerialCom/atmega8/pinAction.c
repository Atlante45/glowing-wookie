#include "pinAction.h"
#include <avr/io.h>
#include "../common/bits.h"


void digital_write(int pinID, int value)
{
  char mask = 0;
  binary_write(&mask, pinID%8, 1, 1);
  char val = 0;
  binary_write(&val, pinID%8, 1, value);

  if (pinID<8)
    {
      DDRD |= mask;
      PIND = val & mask;
    }
  else if (pinID<13 && pinID>7)
    {
      DDRB |= mask;
      PINB = val & mask;
    }
  else if (pinID>13)
    {
      DDRC |= mask;
      PINC = val & mask;
    }
  else
    return -1;


}

int digital_read(int pinID)
{
  char mask = 0;
  binary_write(&mask, pinID%8, 1, 1);
 
  if (pinID<8)
    {
      DDRD |= mask;
      return PIND & mask;
    }
  else if (pinID<13 && pinID>7)
    {
      DDRB |= mask;
      return PINB & mask;
    }
  else if (pinID>13)
    {
      DDRC |= mask;
      return PINC & mask;
    }
  else
    return -1;
}

double analog_read(int pinID)
{  
    char mask = 0;
    binary_write(&mask, pinID%8, 1, 1);

    if (pinID>13)
      {
	DDRC |= mask;
	return PINC & mask;
      }

    else
      return -1;
}


