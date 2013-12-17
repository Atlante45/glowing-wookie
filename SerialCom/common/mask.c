#include "../common/mask.h"
#include "../common/bits.h"

#include <stdlib.h>

#ifndef DISABLE_EXTRA_FUNCTIONS
#include <stdio.h>
#include <string.h>
#endif


/**
 * Allocation of a new mask structure.
 * \param nb_values Number of values contained in the mask.
 * \param value_size Size (in bits !) of a value.
 * \return Returns a mask structure.
 */
mask_t *mask__new(unsigned int nb_values, unsigned int value_size){
  struct mask *m = (struct mask*) malloc(sizeof(*m));
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
 * Writes a string representing the mask m in output_string at a specific
 * offset position in bits.
 * \param m mask to write in a string
 * \param output_string string where the output will be written (should be allocated)
 * \param offset start position in bits in the string output_string
 * \return Returns an error code.
 */
int mask__to_string(mask_t *m, char *output_string, unsigned int offset){
  if (m == NULL || output_string == NULL)
     return MASK__ERROR_INVALID_PARAMETER;

  int i;
  for (i = 0; i < m->nb_values; i++){
    binary_write(output_string, offset + i * m->value_size, m->value_size, m->values[i]);
  }

  return MASK__SUCCESS;
}

/**
 * Converts a string representing a mask to a mask structure given
 * the number of values and their size in the mask.
 * \param input_string string representing a mask
 * \param nb_values number of values in the mask
 * \param value_size size in bits of each value
 * \param offset start position in bits in the string input_string
 * \return a mask structure
 */
mask_t *mask__from_string(char *input_string, unsigned int nb_values, unsigned int value_size, unsigned int offset){
  mask_t *m = mask__new(nb_values, value_size);
  if (m == NULL) 
    return NULL;

  int i;
  for (i = 0; i < nb_values; i++){
    m->values[i] = binary_read(input_string, offset + m->value_size * i, m->value_size);
  }

  return m;
}

#ifndef DISABLE_EXTRA_FUNCTIONS
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
    binary_sprint(buffer, m->values[i], m->value_size);
    sprintf(line, "%s | %*d", line, width, m->values[i]);
    strcat(line2, " | "); strcat(line2, buffer);
  }
  printf(" |\n ");
  for (i = 0; i < (width + 3) * m->nb_values + 1; i++)
    printf("=");
  printf("\n%s |\n%s |\n", line, line2);
}
#endif

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

/**
 * Returns the size in bits of the mask string
 * @param m
 * @return
 */
unsigned int mask__get_size(mask_t *m){
    return m->nb_values * m->value_size;
}

/**
 * Returns the (rounded) size in bytes of the mask string
 * @param m
 * @return
 */
unsigned int mask__get_length(mask_t *m){
    unsigned int size = m->nb_values * m->value_size;
    if (size % 8 == 0)
        return size / 8;
    else
        return size / 8 + 1;
}
