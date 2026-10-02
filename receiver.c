#include <stdio.h>
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
char buffer[1024];
while (1) {
  int n=recvfrom(sockfd,buffer,sizeof(buffer)-1,0,NULL,NULL);

  if(n<0) return 1;

  buffer[n] = '\0';
  printf("Received: %s\n",buffer);
}
}
