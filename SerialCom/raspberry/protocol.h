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
    void receiveCommand(int &command, int &reply_code,
			int &payload_size, char *payload);

    /* synchronous */
    int getCaps(int &output__nb_pins);
    int reset();
    int ping(int &output__protocol_version);
    int read(enum types type, mask_t *pins, mask_t *output__values);
    int write(enum types type, mask_t *pins, mask_t *values);
    int setType(mask_t *pins, mask_t *states);
    int getType(mask_t *pins, mask_t *output__type_mask);
    void getFailSafe(mask_t *pins);
    int setFailSafe(int timeout, enum types type,
			 mask_t *pins, mask_t *values);

    /* asynchronous */
    void sendGetCaps();
    void sendReset();
    void sendPing();
    void sendRead(enum types type, mask_t *pins);
    void sendWrite(enum types type, mask_t *pins, mask_t *values);
    void sendSetType(mask_t *pins, mask_t *states);
    void sendGetType(mask_t *pins);
    void sendGetFailSafe(mask_t *pins);
    void sendSetFailSafe(int timeout, enum types type,
			 mask_t *pins, mask_t *values);
private:
    int  parse(int &command, int &reply_code,
	       int &payload_size, char *payload);
    serialib *port;
    int timeout;
};

#endif /* _PROTOCOLE_H_ */
