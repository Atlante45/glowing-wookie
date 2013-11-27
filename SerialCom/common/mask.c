#include "mask.h"
#include "bits.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/**
 * Allocation of a new mask structure.
 * \param nb_values Number of values contained in the mask.
 * \param value_size Size (in bits !) of a value.
 * \return Returns a mask structure.
 */
mask_t *mask__new(unsigned int nb_values, unsigned int value_size){
  struct mask *m = (mask*) malloc(sizeof(*m));
  if (m == NULL)
    return NULL;
  m->values = (int*) malloc(sizeof(*(m->values)) * nb_values);
  if (m->values == NULL){
    free (m);
    return NULL;
  }
    
  m->nb_values  = nb_values;
  m->value_size = value_size;
    
  return m;
}

/**
 * Frees the mask structure memory.
 * \param m mask
 */
void mask__free(mask_t *m){
  if (m != NULL)
    free (m->values);
  free(m);
}

/**
 * Writes a string representing the mask m in output_string and
 * checks if output_string is long enough to contain the mask.
 * \param m mask to write in a string
 * \param output_string string where the output will be written (should be allocated)
 * \param output_string_length allocated length of the output_string
 * \return Returns an error code.
 */
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

/**
 * Converts a string representing a mask to a mask structure given
 * the number of values and their size in the mask.
 * \param input_string string representing a mask
 * \param nb_values number of values in the mask
 * \param value_size size in bits of each value
 * \return a mask structure
 */
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

/**
 * Displays a mask to the standard output.
 * \param m mask
 */
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

/**
 * If the mask contains only one non-zero value,
 * returns the index of the non-zero value, or
 * -1 otherwise.
 * NB: can be used to select mask functions
 * from non-mask functions.
 * \param m mask
 */
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
