#ifndef _PROTOCOLE_H_
#define _PROTOCOLE_H_

#include <stdlib.h>

#define NB_PINS 24

enum mode {
  SYNC = 0,
  ASYNC,

  NUM_MODE
};


int  getProtocolVersion();
void reset();
void failSafe(/* TODO */);
void setMode(enum mode mode);
void setHeartBeat(char freq); // 0 <= freq < 16 --> frequence réel = 2^freq
void heartBeat();
int setState();
int getState();
int read();
int write();
int monitorRead();

#endif /* _PROTOCOLE_H_ */
