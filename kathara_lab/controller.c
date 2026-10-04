#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>

int main(){
  while(1){
    int status=system("kathara exec h1 \"ping -c 1 -W 1 10.0.2.2\" >/dev/null 2>&1");

    if(status==0){
      printf("Path through r1 is alive\n");
    }
    else{
      printf("Path is through r1 is DOWN\n");
      system("kathara exec h1 \"ip route replace 10.0.4.2/32 via 10.0.1.253\"");
      system("kathara exec h2 \"ip route replace 10.0.1.0/24 via 10.0.3.254\"");
      printf("Switched traffic through r2\n");
      break;
    }
    sleep(1);
  }

  return 0;
}

