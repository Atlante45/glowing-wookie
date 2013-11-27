#include "bits.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

char *binary_set(char *c, unsigned char bit, char bool_value){
  printf("binary_set: bit=%d, v=%u\n", bit, bool_value);
  c += bit / 8;
  unsigned char v = 1 << 7-(bit % 8);
  if (bool_value)
    *c |= v;
  else
    *c &= ~v;
  return c;
}

char binary_get(char c, unsigned char start_pos, unsigned char nb_bits){
  c >>= (8 - start_pos - nb_bits);
  c &= (1 << nb_bits) - 1;  
  return c;
}

/**
 * Writes in the string 'output_string'
 * the first 'nb_bits' least significant bits 
 * of the number 'value' in binary from position 'start_position'.
 * /return pointer to output_string
 */
char *binary_write(char *output_string, int start_position, int nb_bits,  int value){
  if (output_string != NULL){
    int z;
    for (z = 1 << (nb_bits - 1); z > 0; z >>= 1)
      binary_set(output_string, start_position++, (value & z) == z);
  }
  return output_string;
}

/**
 * Parse the first 'nb_bits' from 'start_position' of
 * the buffer string.
 * \return Returns the parsed value.
 */
int binary_read(char *input_string, int start_position, int nb_bits){
  if (input_string == NULL)
    return 0;

  input_string += start_position / 8;
  start_position %= 8;

  int v = 0; int nb_written = 0;
  while (nb_bits){
    char len = nb_bits;
    if (len >= 8 - start_position) 
      len = 8 - start_position;

    v <<= len;
    v += binary_get(input_string[0], start_position, len);

    printf("* v=%d start=%d nb_bits=%d len=%d written=%d input=%u\n",v,start_position, nb_bits, len, nb_written, input_string[0]);
    nb_written = len;
    nb_bits -= len;
    start_position = 0;
    input_string++;
  }


  return v;
}

void main(){
  unsigned char buffer[8]={0,0,0,0,0,0,0,0};


  binary_write(&(buffer[0]), 8+5, 31, 8461468);
  
  int i;
  for (i=0; i < 8; i++)
    printf("%u,", buffer[i]);
  
  printf("\n%d\n", binary_read(&(buffer[0]), 8+5,31));
}
