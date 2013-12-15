#ifndef BITS_H_
#define BITS_H_

/* single bit operation */
char *binary_set   (char *string, unsigned char bit_position, char bool_value);
/* single byte operation */
char  binary_get   (char c, unsigned char start_pos, unsigned char nb_bits);

/* multi-byte operations */
char *binary_write (char *output_string, int start_position,
		    int nb_bits, int value);
int   binary_read  (char *input_string,  int start_position, int nb_bits);

/* print functions */
char *binary_sprint(char *output_string, int nb_bits,  int value);
void  binary_print (int nb_bits, int value);


#endif

