#include "checksum.h"

int checksum(char *buffer, int length) {
    int i = 0;
    unsigned char checksum = 0;
    for (i = 0; i < length; ++i) {
        checksum += buffer[i];
    }

    return ((unsigned int) (~checksum)) & ((1 << 8) - 1);
}
