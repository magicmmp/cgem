#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#define SIZE 65536
 
int main(int argc,char *argv[])
{  
   FILE *fw = NULL;
   FILE *fd = NULL;
   fw = fopen("data_check.txt","w+");
   char msg[128];
   unsigned char buff[65536];
   int i,j,data_len,no;
   int chid,tigerid;
   int flag;
   int err_count,index,index0;
   long long D[6],H0,H1;

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

   if (argc !=3)
   {   
      printf("\nsingle channel data check,input: tiger_id,chid\n");
      exit(0);
   }
   else
   {
      tigerid =atoi(argv[1]);
      chid = atoi(argv[2]);  
   }
   
   err_count=0;
   no=0;
   index=0;
//   int start=0;
   while(err_count<32)
   {
     data_len=recvfrom(socket_descriptor,buff,SIZE,0,(struct sockaddr *)&cliaddr,&sin_len);
     no++;
     i=0;
     while((buff[i+7]>>3==0x04)&&(i<data_len-6))
       i=i+8;
//     if(start==0)
//        start=1;
     for(H0=0;i<data_len-7;i=i+8)
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
       D[index]=H1;
       H0=H1;
       if(index==2||index==5)
       { 
         flag=0;
         if((D[index-2]>> 56& 0x7)!=tigerid||(D[index-1]>> 56& 0x7)!=tigerid||(D[index]>> 56& 0x7)!=tigerid)
           flag=1;
         if((D[index-2]>> 48& 0x3F)!=chid||(D[index-1]>> 48& 0x3F)!=chid)
           flag=1;
         if((D[index-1]>> 30& 0xFFFF)-(D[index-2]>> 30& 0xFFFF)!=0x24)
           flag=1;
         if((D[index-1]>> 20& 0x3FF)-(D[index-2]>> 20& 0x3FF)!=0x24)
           flag=1;
         if(flag)
         {
           err_count++;
           printf("no=%d,i=%d, err_count=%d\n",no,i,err_count);
           printf("tigerid=%d,  %d,%d,%d\n",tigerid,D[index-2]>> 56& 0x7,D[index-1]>> 56& 0x7,D[index]>> 56& 0x7);
           printf("chid = %d ,  %d,%d\n",chid,D[index-2]>> 48& 0x3F,D[index-1]>> 48& 0x3F);
           printf("tcos1-tcos0=%0x, tcos1=%0x, tcos0=%0x\n",(D[index-1]>> 30& 0xFFFF)-(D[index-2]>> 30& 0xFFFF),D[index-1]>> 30& 0xFFFF,D[index-2]>> 30& 0xFFFF);
           printf("ecos1-ecos0=%0x, ecos1=%0x, ecos0=%0x\n",(D[index-1]>> 20& 0x3FF)-(D[index-2]>> 20& 0x3FF),D[index-1]>> 20& 0x3FF,D[index-2]>> 20& 0x3FF);
           j=sprintf(msg,"tigerid or chid not equal.input tigerid=%d, chid=0X%02X, i=%d, err_count=%d\n",tigerid,chid,i,err_count);
           fwrite(msg,sizeof(unsigned char),j,fw);
           j=sprintf(msg,"%08X%08X TIGER %01X: EW: ChID: %02X tacID: %01X Tcoarse: %04X Ecoarse: %03X Tfine: %03X Efine: %03X\n",D[index-2]>>32,D[index-2]&0xFFFFFFFF,(D[index-2]>> 56)& 0x7,(D[index-2]>> 48)& 0x3F,(D[index-2]>>46)& 0x3,(D[index-2]>>30)&0xFFFF,(D[index-2]>> 20)& 0x3FF,(D[index-2]>> 10)& 0x3FF,D[index-2] & 0x3FF);
           fwrite(msg,sizeof(unsigned char),j,fw);
           printf("%s\n",msg);
           j=sprintf(msg,"%08X%08X TIGER %01X: EW: ChID: %02X tacID: %01X Tcoarse: %04X Ecoarse: %03X Tfine: %03X Efine: %03X\n",D[index-1]>>32,D[index-1]&0xFFFFFFFF,(D[index-1]>> 56)& 0x7,(D[index-1]>> 48)& 0x3F,(D[index-1]>>46)& 0x3,(D[index-1]>>30)&0xFFFF,(D[index-1]>> 20)& 0x3FF,(D[index-1]>> 10)& 0x3FF,D[index-1] & 0x3FF);
           fwrite(msg,sizeof(unsigned char),j,fw);
           printf("%s\n",msg);
           j=sprintf(msg,"%08X%08X TIGER %01X: HB: Framecount: %08X SEUcount: %08X\n",D[index]>>32,D[index]&0xFFFFFFFF,(D[index]>> 56)& 0x7,(D[index]>> 15)& 0xFFFF,D[index]& 0x7FFF) ;
           fwrite(msg,sizeof(unsigned char),j,fw);
           printf("%s\n",msg);
         }
         if(index==5)
           index0=2;
         else
           index0=5;
         if((D[index]>> 15& 0xFFFF)-(D[index0]>> 15& 0xFFFF)!=1&&(D[index]>> 15& 0xFFFF)!=0x0)
         {
           err_count++;
           printf("frame1-frame0=%d\n",(D[index]>> 15& 0xFFFF)-(D[index0]>> 15& 0xFFFF));
           printf("no=%d,i=%d, err_count=%d\n",no,i,err_count);
            j=sprintf(msg,"frame1-frame0=%d, i=%d, err_count=%d\n",(D[index]>> 15& 0xFFFF)-(D[index0]>> 15& 0xFFFF),i,err_count);
           fwrite(msg,sizeof(unsigned char),j,fw);
           j=sprintf(msg,"%08X%08X TIGER %01X: EW: ChID: %02X tacID: %01X Tcoarse: %04X Ecoarse: %03X Tfine: %03X Efine: %03X\n",D[index0-2]>>32,D[index0-2]&0xFFFFFFFF,(D[index0-2]>> 56)& 0x7,(D[index0-2]>> 48)& 0x3F,(D[index0-2]>>46)& 0x3,(D[index0-2]>>30)&0xFFFF,(D[index0-2]>> 20)& 0x3FF,(D[index0-2]>> 10)& 0x3FF,D[index0-2] & 0x3FF);
           fwrite(msg,sizeof(unsigned char),j,fw);
           printf("%s\n",msg);
           j=sprintf(msg,"%08X%08X TIGER %01X: EW: ChID: %02X tacID: %01X Tcoarse: %04X Ecoarse: %03X Tfine: %03X Efine: %03X\n",D[index0-1]>>32,D[index0-1]&0xFFFFFFFF,(D[index0-1]>> 56)& 0x7,(D[index0-1]>> 48)& 0x3F,(D[index0-1]>>46)& 0x3,(D[index0-1]>>30)&0xFFFF,(D[index0-1]>> 20)& 0x3FF,(D[index0-1]>> 10)& 0x3FF,D[index0-1] & 0x3FF);
           fwrite(msg,sizeof(unsigned char),j,fw);
           printf("%s\n",msg);
           j=sprintf(msg,"%08X%08X TIGER %01X: HB: Framecount: %08X SEUcount: %08X\n",D[index0]>>32,D[index0]&0xFFFFFFFF,(D[index0]>> 56)& 0x7,(D[index0]>> 15)& 0xFFFF,D[index0]& 0x7FFF) ;
           fwrite(msg,sizeof(unsigned char),j,fw);  
           printf("%s\n",msg);
           j=sprintf(msg,"%08X%08X TIGER %01X: EW: ChID: %02X tacID: %01X Tcoarse: %04X Ecoarse: %03X Tfine: %03X Efine: %03X\n",D[index-2]>>32,D[index-2]&0xFFFFFFFF,(D[index-2]>> 56)& 0x7,(D[index-2]>> 48)& 0x3F,(D[index-2]>>46)& 0x3,(D[index-2]>>30)&0xFFFF,(D[index-2]>> 20)& 0x3FF,(D[index-2]>> 10)& 0x3FF,D[index-2] & 0x3FF);
           fwrite(msg,sizeof(unsigned char),j,fw);
           printf("%s\n",msg);
           j=sprintf(msg,"%08X%08X TIGER %01X: EW: ChID: %02X tacID: %01X Tcoarse: %04X Ecoarse: %03X Tfine: %03X Efine: %03X\n",D[index-1]>>32,D[index-1]&0xFFFFFFFF,(D[index-1]>> 56)& 0x7,(D[index-1]>> 48)& 0x3F,(D[index-1]>>46)& 0x3,(D[index-1]>>30)&0xFFFF,(D[index-1]>> 20)& 0x3FF,(D[index-1]>> 10)& 0x3FF,D[index-1] & 0x3FF);
           fwrite(msg,sizeof(unsigned char),j,fw);
           printf("%s\n",msg);
           j=sprintf(msg,"%08X%08X TIGER %01X: HB: Framecount: %08X SEUcount: %08X\n",D[index]>>32,D[index]&0xFFFFFFFF,(D[index]>> 56)& 0x7,(D[index]>> 15)& 0xFFFF,D[index]& 0x7FFF) ;
           fwrite(msg,sizeof(unsigned char),j,fw);      
           printf("%s\n",msg);
       
         }
       }
       index=(index+1)%6;
     }
    }
   printf("exit.\n");
   fclose(fw);
   close(socket_descriptor);
}
