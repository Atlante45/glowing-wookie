#include "protocol.h"
#include "communication.h"

#include <stdio.h>

int main(int argc, char * argv[]){
  state current;
  current.reply_id=0;
  init_com();

  char buffer;
  while(1){
    parseProtocol(&current);
    //if (recv_msg(&(buffer), 1)>0)
    //  printf("hello");
  }
  close_com();
}
