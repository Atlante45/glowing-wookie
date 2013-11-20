#ifndef _PROTOCOLE_H_
#define _PROTOCOLE_H_

#include <cstdlib>
#include "../common/mask.h"

#define NB_PINS 24

enum Mode {
  SYNC = 0,
  ASYNC,

  NUM_MODE
};

void getProtocolVersion();
void reset(char resetType);
void failSafe(/* TODO */);
void setMode(Mode Mode);
void setHeartBeat(char freq); // 0 <= freq < 16 --> frequence réel = 2^freq
void heartBeat();
void setState(mask_t *pins, mask_t *states);
void getState(mask_t *pins);
void read();
void write();
void monitorRead();

#endif /* _PROTOCOLE_H_ */
