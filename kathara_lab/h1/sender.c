#include <stdio.h>
#include <errno.h>
#include <sys/time.h>
#include "protocol.h"
#include <strings.h>
#include<unistd.h>
#include <arpa/inet.h>

int main(){
  int sockfd;

  sockfd=socket(AF_INET,SOCK_DGRAM,0);
  if(sockfd<0) return 1;

  struct timeval timer;
  timer.tv_sec=1;
  timer.tv_usec=0;
  setsockopt(sockfd,SOL_SOCKET,SO_RCVTIMEO,&timer,sizeof(timer));

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
      pkt.length=0;
      pkt.sequence_number=s;
      sendto(sockfd,&pkt,sizeof(pkt),0,(struct sockaddr *) &receiver,sizeof(receiver));
      struct ack ack;

      int acknow=0;

      while(!acknow){
        int r=recvfrom(sockfd,&ack,sizeof(ack),0,NULL,NULL);
        if(r<0){
          if(errno==EAGAIN || errno==EWOULDBLOCK){
            printf("Timeout for FIN packet, retransmitting.....");
            sendto(sockfd,&pkt,sizeof(pkt),0,(struct sockaddr*)&receiver,sizeof(receiver));
          }
          else{
            perror("recvfrom");
            return 1;
          }
        }
        else{
          if(ack.sequence_number==pkt.sequence_number){
            printf("FIN ack received\n");
            acknow=1;
          }
        }
      }
      printf("Entire file sent\n");
      break;
    }

    else{
       pkt.type=TYPE_D; 
       pkt.sequence_number=s++;
       pkt.length=n;

       sendto(sockfd,&pkt,sizeof(pkt),0,(struct sockaddr *)& receiver,sizeof(receiver));

       struct ack ack;
       int acknowlegement=0;

       while(!acknowlegement){
         int r = recvfrom(sockfd,&ack,sizeof(ack),0,NULL,NULL);
         if(r<0){
           if(errno==EAGAIN || errno==EWOULDBLOCK){
             printf("Timeout for packet, retransmitting packet %d ......",pkt.sequence_number);

             sendto(sockfd,&pkt,sizeof(pkt),0,(struct sockaddr *)&receiver,sizeof(receiver));
             printf("Retransmission done\n");

             continue;
           }
           perror("recvfrom");
           return 1;
         }
         if(pkt.sequence_number==ack.sequence_number){
           printf("ACK %d received successfully\n",ack.sequence_number);
           acknowlegement=1;
         }
       }
      }
   }  
  fclose(fp);
  close(sockfd);
}
