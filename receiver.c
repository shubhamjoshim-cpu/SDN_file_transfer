#include <stdio.h>
#include "protocol.h"

#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

  
int main(){

int sockfd;
sockfd=socket(AF_INET, SOCK_DGRAM, 0);

if(sockfd<0) return 1;

struct sockaddr_in server_addr;
memset( &server_addr,0,sizeof(struct sockaddr_in));

server_addr.sin_family = AF_INET;
server_addr.sin_port=htons(8000);
server_addr.sin_addr.s_addr=INADDR_ANY;

int a=bind(sockfd,(struct sockaddr *) &server_addr,sizeof(struct sockaddr_in));
if(a<0) return 1;

FILE *fp=fopen("receiver.txt","wb");
if(fp==NULL){
  perror("file reader failed");
  return 1;
}
struct packet pkt;
while(1){
  int n=recvfrom(sockfd,&pkt,sizeof(pkt),0,NULL,NULL);
  if(n<0){
      perror("receiver failed");
  return 1;
}
if(pkt.type==TYPE_F) {
  printf("End of file being sent");
  break;
}
else if(pkt.type==TYPE_D){
  fwrite(pkt.data,1,pkt.length,fp);
  printf("Received %dth packet of bytes: %d\n",pkt.sequence_number,pkt.length);
}
  }
fclose(fp);
close(sockfd);

}
