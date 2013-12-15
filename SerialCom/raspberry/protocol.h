#ifndef _PROTOCOLE_H_
#define _PROTOCOLE_H_

#include <cstdlib>
#include "../common/mask.h"

#define HOST_PROTOCOL_VERSION 1

void getCaps();
void reset();
void ping();
void read(mask_t *pins);
void write(mask_t *pins, mask_t *values);
void setType(mask_t *pins, mask_t *states);
void getType(mask_t *pins);
void getFailSafe();
void setFailSafe();

#endif /* _PROTOCOLE_H_ */
