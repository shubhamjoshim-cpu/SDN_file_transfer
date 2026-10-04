#include <stdio.h>
#include <string.h>
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

  char ip[32];

  int choice;

  while(1){
    printf("Choose the destination: \n1.Local Transfer\n2.Network Transfer (host 2)\n");
    scanf("%d",&choice);
    if(choice<1 || choice >2){
      printf("Invalid input try again\n");
    }
    else if(choice == 1){
      strcpy(ip,"127.0.0.1");
      break;

    }
    else{
      strcpy(ip,"10.0.4.2");
      break;
    }
  }
  inet_pton(AF_INET,ip,&receiver.sin_addr);


  char name[256];
  printf("Enter the file name: ");
  scanf("%255s",name);

  FILE *fp=fopen(name,"rb");
  if(fp==NULL){
    perror("Error opening file");
    return 1;
  }

  struct packet pkt;
  int s=0;

  while(1) {

    int acknow=0;

    if(s==0){
      pkt.type=TYPE_H;
      strcpy(pkt.data,name);
      pkt.length=strlen(name)+1;
      pkt.sequence_number=s++;
      sendto(sockfd,&pkt,sizeof(pkt),0,(struct sockaddr *) &receiver,sizeof(receiver));
      struct ack ack;

      //wait for ack
      while(!acknow) {
        int r=recvfrom(sockfd,&ack,sizeof(ack),0,NULL,NULL);
        if(r<0){
          if(errno==EAGAIN || errno==EWOULDBLOCK){
            printf("Timeout for packet, retransmitting packet %d ......",pkt.sequence_number);

            sendto(sockfd,&pkt,sizeof(pkt),0,(struct sockaddr *)&receiver,sizeof(receiver));
            printf("Retransmission done\n");

          }
          else{
            perror("recvfrom");
            return 1;
          }
        }
        else{
          if(pkt.sequence_number==ack.sequence_number){
            printf("Header ACK received\n");
            acknow=1;
          }
        }
      }
      continue; 
    }

    size_t n=fread(pkt.data,1,PAYLOAD_SIZE,fp);

    if(n==0){
      pkt.type=TYPE_F;
      pkt.length=0;
      pkt.sequence_number=s;
      sendto(sockfd,&pkt,sizeof(pkt),0,(struct sockaddr *) &receiver,sizeof(receiver));
      struct ack ack;


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
