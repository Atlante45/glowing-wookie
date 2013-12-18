#include "communication.h"


#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

#include <stdio.h>

#define FIFO_IN  "../to_atmega"
#define FIFO_OUT "../to_raspberry"

int fifo_in;
int fifo_out;

void init_com(){
  fifo_in = open(FIFO_IN, O_RDONLY);
  fifo_out = open(FIFO_OUT, O_WRONLY);
}

void close_com(){
  close(fifo_out);
  close(fifo_in);
}

void send_msg(char * msg, int length){
   write(fifo_out, msg, length);
}

int recv_msg(char * buffer, int length){
  int nb_read = 0;
  while( nb_read < length){
    int r = read(fifo_in, buffer+nb_read, length-nb_read);
    nb_read += r;
  }
  return nb_read; 
}

int is_ready_to_read(){
  return 1;
}

int is_ready_to_send(){
  return 1;
}
