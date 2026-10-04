#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>

int main(){

  int active_path=1;

  while(1){
    int a=0;
    int b=0;
    int status1;
    int status2;
    for(int i=0;i<3;i++){
     status1=system("kathara exec h1 \"ping -c 1 -W 1 10.0.2.2\" >/dev/null 2>&1");
     status2=system("Kathara exec h1 \"ping -c 1 -W 1 10.0.3.2\" >/dev/null 2>&1 ");
     if(status1!=0) a++;
     if (status2!=0) b++;

    }
    if(a!=3 && b!=3){
    printf("Path through r1 and r2 are alive\n");
}
    else if (b==3 && a!=3) {
      printf("Path through r2 is DOWN, using r1\n");

      if (active_path != 1) {
          system("kathara exec h1 \"ip route replace 10.0.4.2/32 via 10.0.1.254\"");
          system("kathara exec h2 \"ip route replace 10.0.1.0/24 via 10.0.2.254\"");

          active_path = 1;
          printf("Switched traffic through r1\n");
      }
  }
    else if(b==3 && a==3){
      printf("Both the Paths are DOWN cannot transmit data");
      system("kathara exec h1 \"pkill -x sender\"");
      system("kathara exec h2 \"pkill -x receiver\"");
      break;
    }
    else if (a==3 && b!=3){
      printf("Path is through r1 is DOWN\n");
        if(active_path!=2){
        system("kathara exec h1 \"ip route replace 10.0.4.2/32 via 10.0.1.253\"");
        system("kathara exec h2 \"ip route replace 10.0.1.0/24 via 10.0.3.254\"");
        printf("Switched traffic through r2\n");
        active_path=2;
      }
    }
    sleep(1);
  }

  return 0;
}

