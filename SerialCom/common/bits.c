#include "bits.h"

#include <stdlib.h>
#include <string.h>

/**
 * Writes in the string 'output_string'
 * the first 'nb_bits' least significant bits 
 * of the number 'value' in binary.
 * /return pointer to output_string
 */
char *binary_print(char *output_string, int value, int nb_bits){
  if (output_string != NULL){
    int z;
    char *c = output_string;
    for (z = 1 << (nb_bits - 1); z > 0; z >>= 1)
      *(c++) = ((value & z) == z) ? '1' : '0';
    *c = '\0';
  }
  return output_string;
}

/**
 * Parse the first 'nb_bits' characters of
 * the 'string' as a binary number.
 * \return Returns the parsed value.
 */
int binary_parse(char *string, int nb_bits){
  char *end_p;
  static char buffer[1024];
  buffer[0] = '\0';
  strncpy(buffer, string, nb_bits);
  unsigned long int v = strtoul(buffer, &end_p, 2);
  return (int)v;
}
