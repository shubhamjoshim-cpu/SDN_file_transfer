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

  FILE *fp=fopen("test.txt","rb");

  char buffer[1024];
  size_t n;
  while(1){
    n=fread(buffer,1,sizeof(buffer),fp);
    if(n==0) break;
    sendto(sockfd,buffer,n,0,(struct sockaddr * )&receiver,sizeof(receiver));
    printf("Read %zu bytes from sender",n);
  }
  fclose(fp);

  char eof[]="__EOF__";
  sendto(sockfd,eof,strlen(eof),0,(struct sockaddr *)&receiver,sizeof(receiver));
  return 0;

}
