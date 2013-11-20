#include "mask.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


mask_t *mask__new(unsigned int nb_values, unsigned int value_size){
  struct mask *m = malloc(sizeof(*m));
  if (m == NULL)
    return NULL;
  m->values = malloc(sizeof(*(m->values)) * nb_values);
  if (m->values == NULL){
    free (m);
    return NULL;
  }
    
  m->nb_values  = nb_values;
  m->value_size = value_size;
    
  return m;
}

void mask__free(mask_t *m){
  if (m != NULL)
    free (m->values);
  free(m);
}

int mask__to_string(mask_t *m, char *output_string, unsigned int output_string_length){
  if (m == NULL || output_string == NULL)
    return MASK__ERROR_INVALID_PARAMETER;

  unsigned int mask_length = m->value_size * m->nb_values;
  if (mask_length + 1 > output_string_length)
    return MASK__ERROR_INDEX_OUT_OF_RANGE;

  int i;
  char *s = output_string;
  for (i = 0; i < m->nb_values; i++){
    binary_print(s, m->values[i], m->value_size);
    s += m->value_size;
  }
  (*s) = '\0';

  return MASK__SUCCESS;
}

mask_t *mask__from_string(char *input_string, unsigned int nb_values, unsigned int value_size){
  unsigned int mask_length  = nb_values * value_size;
  int string_length = strnlen(input_string, mask_length + 1);

  if (mask_length > string_length)
    return NULL;
  
  mask_t *m = mask__new(nb_values, value_size);
  if (m == NULL) 
    return NULL;

  int i;
  char *s = input_string;
  for (i = 0; i < nb_values; i++){
    m->values[i] = binary_parse(s, m->value_size);
    s += m->value_size;
  }

  return m;
}

void mask__display(struct mask *m){
  if (m == NULL){
    printf(" Null pointer, no mask to display.\n");
    return;
  }

  printf(" Mask of %d values of %d bits : \n", m->nb_values, m->value_size);

  char buffer[30];
  sprintf(buffer, "%d", m->nb_values - 1);
  int width = strlen(buffer);
  if (m->value_size > width) 
    width = m->value_size;

  int i;
  
  char line [128] = "";
  char line2[128] = "";
  for (i = 0; i < m->nb_values; i++){
    printf(" | %*d", width, i);
    binary_print(buffer, m->values[i], m->value_size);
    sprintf(line, "%s | %*d", line, width, m->values[i]);
    strcat(line2, " | "); strcat(line2, buffer);
  }
  printf(" |\n ");
  for (i = 0; i < (width + 3) * m->nb_values + 1; i++)
    printf("=");
  printf("\n%s |\n%s |\n", line, line2);
}

int mask__single_value_index (struct mask *m){
  int index = -1, i;
  for (i = 0; i < m->nb_values; i++){
    if (m->values[i] != 0){
      if (index < 0)
	index = i;
      else
	return -1;
    }
  }
  
  return index;
}


int binary_parse(char *string, int nb_bits){
  char *end_p;
  static char buffer[1024];
  buffer[0] = '\0';
  strncpy(buffer, string, nb_bits);
  unsigned long int v = strtoul(buffer, &end_p, 2);
  return (int)v;
}

int binary_print(char *output_string, int value, int nb_bits){
  if (output_string == NULL)
    return MASK__ERROR_INVALID_PARAMETER;

  int z;
  char *p = output_string;
  for (z = 1 << (nb_bits - 1); z > 0; z >>= 1)
    *(p++) = ((value & z) == z) ? '1' : '0';
  *p = '\0';

  return MASK__SUCCESS;
}

/*
void main(){
  char buffer[100] = "---------------------------------";
  mask_t *m = mask__new(5, 10);
  m->values[0] = 1;
  m->values[1] = 2;
  m->values[2] = 3;
  m->values[3] = 4;

  mask__display(m);

  mask__to_string(m, buffer, 100);
  printf("%s\n", buffer);
  mask_t *m2 = mask__from_string(buffer, 5, 10);
  mask__display(m2);

  mask_t *m3 = mask__new(5, 10);
  m3->values[0] = 0;
  m3->values[1] = 2;
  m3->values[2] = 0;
  m3->values[3] = 0;
  printf("\nindex=%d\n",mask__single_value_index(m3));

  mask__free(m);
  mask__free(m2);

}
//*/
