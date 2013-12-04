#include "bits.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/**
 * Access the *single* 'bit'-nth bit in the string c and changes its value to
 * 'bool_value' ? 1 : 0
 * Bits are numbered from "left to rigth" (most-significant bit first)
 */
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

/**
 * Extracts the 'nb_bits' bits from the 'start_pos'-nth bit in the
 * ***byte*** 'c'. (numbered from "left to right", most-significant bit first)
 * See binary_read for string manipulations.
 */
char binary_get(char c, unsigned char start_pos, unsigned char nb_bits){
  c >>= (8 - start_pos - nb_bits);
  c &= (1 << nb_bits) - 1;  

  char buffer[10]="";
  binary_sprint(buffer, nb_bits, c);
  printf("binary_get: start=%d len=%d %s\n", start_pos, nb_bits, buffer);

  return c;
}

/**
 * Writes in the string 'output_string'
 * the first 'nb_bits' least significant bits 
 * of the number 'value' in binary from position 'start_position'.
 * The 'start_position' in the 'output_string' refers to the
 * 'start_position'-nth bit from "left to right" (most significant bit first)
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
 * the 'intput_string'. (most-significant bits first)
 * \return Returns the parsed value.
 */
int binary_read(char *input_string, int start_position, int nb_bits){
  if (input_string == NULL)
    return 0;

  // start at the right byte :
  input_string += start_position / 8;
  start_position %= 8;

  int v = 0, tmp; 
  while (nb_bits){
    char len = nb_bits; // nb of bits to read globally
    if (len >= 8 - start_position)  // nb of bits to read locally (current byte)
      len = 8 - start_position; 

    v <<= len; // makes empty space to write the current read
    tmp = binary_get(input_string[0], start_position, len); // read
    tmp &= (1 << len) - 1; // warning : when casting signed char to int
    v += tmp; // add current read to global result

    nb_bits -= len; // decrement nb of bits read
    start_position = 0; // start at the beginning of the next byte
    input_string++; // next byte
  }

  
  return v;
}

char *binary_sprint(char *output_string, int nb_bits,  int value){
  if (output_string != NULL){
    int z;
    char *c = output_string;
    for (z = 1 << (nb_bits - 1); z > 0; z >>= 1)
      *(c++) = ((value & z) == z) ? '1' : '0';
    *c = '\0';
  }
  return output_string;
}

void binary_print(int nb_bits, int value){
  char buffer[64]="";
  binary_sprint(buffer, nb_bits, value);
  printf("%s", buffer);
}

/* //TEST
void main(){
  unsigned char buffer[8]={0,0,0,0,0,0,0,0};


  int v=8461468;
  binary_write(&(buffer[0]), 8+5, 31, v);
  
  char out[10]="";
  int i;
  for (i=0; i < 8; i++){
    binary_sprint(&(out[0]), 8, buffer[i]);
    printf("[%d] %s\n", i, out);
  }

 int r =  binary_read(&(buffer[0]), 8+5,31);
  binary_print(31, v); printf("\n");
  printf("\n%d =? %d\n", v, r);

}
*/
