#include <stdio.h>
#include "protocol.h"
#include <strings.h>
#include<unistd.h>
#include <arpa/inet.h>

int main(){
  int sockfd;
  sockfd=socket(AF_INET,SOCK_DGRAM,0);
  if(sockfd<0) return 1;

  struct sockaddr_in receiver;
  receiver.sin_port=htons(8000);
  receiver.sin_family = AF_INET;
  inet_pton(AF_INET,"127.0.0.1",&receiver.sin_addr);
  char message[1024];

  FILE *fp=fopen("test.txt","rb");
  if(fp==NULL){
    perror("Error opening file");
    return 1;
  }
  struct packet pkt;
  int s=0;
while(1) {
  size_t n=fread(pkt.data,1,PAYLOAD_SIZE,fp);
  if(n==0){
    pkt.type=TYPE_F;
    sendto(sockfd,&pkt,sizeof(pkt),0,(struct sockaddr *) &receiver,sizeof(receiver));
    printf("Entire file sent\n");
    break;
  }
  else{
   pkt.type=TYPE_D; 
   pkt.sequence_number=s++;
   pkt.length=n;
   sendto(sockfd,&pkt,sizeof(pkt),0,(struct sockaddr *)& receiver,sizeof(receiver));
  }
  }
fclose(fp);
close(sockfd);
}
