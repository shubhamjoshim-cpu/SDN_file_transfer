#ifndef PROTOCOL_H
#define PROTOCOL_H

#define PAYLOAD_SIZE 1024

#define TYPE_D 1
#define TYPE_F 2

struct packet{
  int type; //type one means the packet is a data packet else it is a fin packet suggesting that the packet is the last one, so the receiver can stop listening
  int sequence_number;//to ensure order
  int length;
  char data[PAYLOAD_SIZE];
};
#endif
