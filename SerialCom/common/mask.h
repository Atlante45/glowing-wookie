#ifndef MASK_H_
#define MASK_H_

#define MASK__SUCCESS                  0
#define MASK__ERROR_INVALID_PARAMETER  1
#define MASK__ERROR_INDEX_OUT_OF_RANGE 2

typedef
struct mask{
  int *values;
  unsigned int nb_values;
  unsigned int value_size;
} mask_t;


mask_t *mask__new         (unsigned int nb_values, unsigned int value_size);
void    mask__free        (mask_t *m);
void    mask__dipslay     (mask_t *m);

int     mask__to_string   (mask_t *m, char *output_string, unsigned int output_string_length);
mask_t *mask__from_string (char *input_string, unsigned int nb_values, unsigned int value_size);

int     binary_print      (char *output_string, int x, int nb_bits);
int     binary_parse      (char *string, int nb_bits);

#endif
