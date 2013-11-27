#ifndef BITS_H_
#define BITS_H_

char *binary_set (char *string, unsigned char bit_position, char bool_value);

char *binary_write (char *output_string, int start_position, int nb_bits, int value);
int   binary_read  (char *input_string,  int start_position, int nb_bits);

#endif

