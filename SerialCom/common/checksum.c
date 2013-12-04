#include "checksum.h"

int checksum(char *buffer, int size) {
    int i = 0;
    unsigned char checksum = 0;
    for (i = 0; i < size; ++i) {
        checksum += buffer[i];
    }

    return (unsigned int) (~checksum);
}
