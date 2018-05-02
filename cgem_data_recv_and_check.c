#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#define SIZE 2048

int main()
{
   char* str;
   FILE *fp = NULL;
   FILE *fw = NULL;
   unsigned char buff[131072],buff2[131072];
   long long int x;
   unsigned int i,j=0;
   unsigned int Total_Data=0;
   unsigned int file_size;
   fp = fopen("FEB0_bin.dat", "rb");
   fw = fopen("data.txt","w+");
   int port=58914;
   char HB[71]="210000005B7C0000 TIGER 1: HB: Framecount: 0000B6F8 SEUcount: 00000000\n";
   char CW[71]="01B79E7F200FC3F0 TIGER 0: CW: ChID: 00  CounterWord: 0000000000000000\n";
   char EW[98]="01B79E7F200FC3F0 TIGER 0: EW: ChID: 00 tacID: 0 Tcoarse: 0000 Ecoarse: 000 Tfine: 000 Efine: 000\n";
/*
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

while(Total_Data<SIZE)
   {
     i=recvfrom(socket_descriptor,buff+Total_Data,2048-Total_Data,0,(struct sockaddr *)&cliaddr,&sin_len);
     Total_Data+=i;
   }
*/
   fseek(fp,0L,SEEK_END);
   file_size=ftell(fp);
   printf("file_size=%u\n",file_size);
   fseek(fp,0L,SEEK_SET);
   fread(buff,1,file_size,fp);
   for(i=0;i<file_size-7;i=i+8)
     for(j=0;j<8;j++)
     {
       buff2[i+j]=buff[i+7-j];
     }
   for(i=0;i<128;i=i+8)
   { 
     x=0;
     for(j=i;j<i+7;j++)
     {
       x=x+buff2[j];
       x=x<<8;
     }
     x=x|buff2[j];
     printf("%016X\n",x);


   }






   fclose(fp);
   fclose(fw);
//   close(socket_descriptor);
}
