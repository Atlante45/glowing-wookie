#include "serialib.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#define FIFO_IN  "../to_raspberry"
#define FIFO_OUT "../to_atmega"

int fifo_in;
int fifo_out;

serialib::serialib(){
  fifo_out = open(FIFO_OUT, O_WRONLY);
  fifo_in = open(FIFO_IN, O_RDONLY);
}

serialib::~serialib(){
  close(fifo_out);
  close(fifo_in);
}

char serialib::Open(const char *Device,const unsigned int Bauds){}
void serialib::Close(){}

char serialib::Write(const void * msg, const unsigned int length){
  return write(fifo_out, msg, length);
}

int serialib::Read(void * buffer, unsigned int length, const unsigned int timeout){
  return read(fifo_in, buffer, length) == length ? 1 : -1;
}
