#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/time.h>
#define SIZE 1048576
 
int main()
{  
   unsigned char buff[65536];
   int i,j=0;
   int Total_Data=0;
   int port=58914;
   int sin_len;
   int socket_descriptor;
   struct sockaddr_in sin,cliaddr;
   bzero(&sin,sizeof(sin));
   sin.sin_family=AF_INET;
   sin.sin_addr.s_addr=inet_addr("192.168.1.200");
   sin.sin_port=htons(port);
   sin_len=sizeof(sin);
   socket_descriptor=socket(AF_INET,SOCK_DGRAM,0);
   bind(socket_descriptor,(struct sockaddr *)&sin,sizeof(sin));
   struct timeval t1, t2 ;
   double timeSpan;
   printf("  data flow test:\n");
   for(i=0;i<5;i++)
   {
     Total_Data=0;
     timeSpan=0;
     gettimeofday(&t1,NULL) ;
     while(Total_Data<SIZE)
     {
       Total_Data+=recvfrom(socket_descriptor,buff,65536,0,(struct sockaddr *)&cliaddr,&sin_len);
      
     }
     gettimeofday(&t2, NULL) ;
     timeSpan = (double)(t2.tv_sec-t1.tv_sec) + (double)t2.tv_usec*1e-6 - (double)t1.tv_usec*1e-6 ;
     printf("    time=%f sec  bytes=%d  BW=%f bytes/sec\n",timeSpan,Total_Data,(Total_Data/timeSpan));
    }
    printf("  finish.\n");
  close(socket_descriptor);
}
