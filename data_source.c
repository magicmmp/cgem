#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
 
int main(int argc,char *argv[])
{  
   FILE *fp = NULL;
   unsigned char buff[66536];
   int i,x,j=0;
   if (argc !=2)
   {
      printf("\ninput x=1 to Vx,else to itself.\n");
      exit(0);
   }
   else
   {
      x =atoi(argv[1]);
   }
   int Total_Data;
   fp = fopen("FEB0_bin_single_chid.dat", "r");
   int port=58914;
   int sin_len;
   int socket_descriptor;
   struct sockaddr_in sin;
   bzero(&sin,sizeof(sin));
   sin.sin_family=AF_INET;
   if(x==1)
     sin.sin_addr.s_addr=inet_addr("192.168.1.201");
   else
     sin.sin_addr.s_addr=inet_addr("127.0.0.1");
   sin.sin_port=htons(port);
   sin_len=sizeof(sin);
   socket_descriptor=socket(AF_INET,SOCK_DGRAM,0);
   fseek(fp,0L,SEEK_END);
   i=ftell(fp);
   fseek(fp,0L,SEEK_SET);
   Total_Data=fread(buff,sizeof(unsigned char),i,fp);
   fclose(fp);
   printf("data size from file=%d\n",Total_Data);
   j=0;
   while(1)
   {
    j=sendto(socket_descriptor,buff,208,0,(struct sockaddr *)&sin,sin_len);
    if(j<0)
     break;
    for(i=0;i<655360;i++);
//  sleep(0.0001);
   }
   printf("exit.\n");
   close(socket_descriptor);
}
