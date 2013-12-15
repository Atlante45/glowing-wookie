#ifndef _PROTOCOLE_H_
#define _PROTOCOLE_H_

#include <cstdlib>
#include "../common/mask.h"
#include "../common/protocol_command.h"
#include "serialib.h"



class Protocol{
public:
    Protocol(serialib *serialPort);
    void sendCommand( char header, char *payload, int payload_length);
    void getCaps();
    void reset();
    void ping();
    void read(enum types type, mask_t *pins);
    void write(enum types type, mask_t *pins, mask_t *values);
    void setType(mask_t *pins, mask_t *states);
    void getType(mask_t *pins);
    void getFailSafe(mask_t *pins);
    void setFailSafe(int timeout, enum types type, mask_t *pins, mask_t *values);
private:
    serialib *port;
};

#endif /* _PROTOCOLE_H_ */
