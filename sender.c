#include <stdio.h>
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
  while(1){
  printf("type your message: ");
  fgets(message,sizeof(message),stdin);
  sendto(sockfd,message,strlen(message),0, (struct sockaddr *) &receiver,sizeof(receiver));
  }
}
