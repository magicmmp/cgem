#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#define SIZE 2048
//#define BUFSIZE = 4096

void sort(char*s,int p1,int p2)
{
    int i,j,k;
    char tmp[8];
    for(i=0;i<p2-p1;i=i+8)
      for(j=p1;j<p2-i;j=j+8)
      {
        if((s[j+6]&0x3F)>(s[j+14]&0x3F))
        {
          for(k=0;k<8;k++)
            tmp[k]=s[j+k];
          for(k=0;k<8;k++)
            s[j+k]=s[j+k+8];
          for(k=0;k<8;k++)
            s[j+k+8]=tmp[k];
        }
      }
}



 
int main()
{  

   FILE *fp = NULL;
   FILE *fw = NULL;
   unsigned char buff[2097152],msg[100];
   int i,j=0;
   long long H0,H1;
   int Total_Data=0;
   fp = fopen("FEB0_bin_single_chid.dat", "r");
   fw = fopen("data.txt","w+");
   fseek(fp,0L,SEEK_END);
   i=ftell(fp);
   fseek(fp,0L,SEEK_SET);
   Total_Data=fread(buff,sizeof(unsigned char),i,fp);
   H0=0;
   printf("Total_Data=%d\n",Total_Data);
   
   for(i=0;i<Total_Data-7;)
   {
     int total=Total_Data-7;
     int p1,p2;
     while(buff[i+7]>>3==0x04&&i<total)
       i=i+8;
     p1=i;
     while(buff[i+7]>>3!=0x04&&i<total)
       i=i+8;
     p2=i-8;
     sort(buff,p1,p2);
     
   }   

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
