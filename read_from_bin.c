#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#define SIZE 2048
//#define BUFSIZE = 4096
 
int main()
{  

   FILE *fp = NULL;
   FILE *fw = NULL;
   unsigned char buff[65536],msg[100];
   int i,j=0;
   long long H0,H1;
   int Total_Data=0;
   fp = fopen("FEB0_bin.dat", "r");
   fw = fopen("data.txt","w+");
   Total_Data=fread(buff,sizeof(unsigned char),16384,fp);
   H0=0;
   printf("Total_Data=%d\n",Total_Data);
   for(i=0;i<Total_Data-7;i=i+8)
   { 
     H1=0;
     for(j=i+7;j>i;j--)
     {
       H1=H1+buff[j];
       H1=H1<<8;  
     }
      H1=H1+buff[j];

     if(H1==H0)
       continue ;
     if((H1>>59) == 0x04)
        j=sprintf(msg,"%08X%08X TIGER %01X: HB: Framecount: %08X SEUcount: %08X\n",H1>>32,H1&0xFFFFFFFF,(H1>> 56)& 0x7,(H1>> 15)& 0xFFFF,H1& 0x7FFF) ;
     if((H1>>59) == 0x08)
        j=sprintf(msg,"%08X%08X TIGER %01X: CW: ChID: %02X  CounterWord: 016X\n",H1>>32,H1&0xFFFFFFFF,(H1>> 56)& 0x7,(H1>> 24)& 0x3F,H1& 0x00FFFFFF);
     if((H1>>59) == 0x00)
        j=sprintf(msg,"%08X%08X TIGER %01X: EW: ChID: %02X tacID: %01X Tcoarse: %04X Ecoarse: %03X Tfine: %03X Efine: %03X\n",H1>>32,H1&0xFFFFFFFF,(H1>> 56)& 0x7,(H1>> 48)& 0x3F,(H1>>46)& 0x3,(H1>>30)&0xFFFF,(H1>> 20)& 0x3FF,(H1>> 10)& 0x3FF,H1 & 0x3FF);
     fwrite(msg,sizeof(unsigned char),j,fw);
     H0=H1;
   }
  
   fclose(fp);
   fclose(fw);
}
