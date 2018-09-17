#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#define SIZE 65536
int physical_data_check(const char*buff, const int &data_len)
{
    long header=0;
    long tailer=0;
    long data=0;
    int res=1;  /*no problem,return res=1,else 0*/
    int i,j;
    int idx=0;
    int IDX=data_len-8;  /*data index should less than IDX,before tailer*/
    if(data_len%8)
        return 0;  /*wrong data length*/
    for(i=idx+7;i>idx;i--)
    {
        header=header+buff[i];
        header=header<<8;
    }
    header=header+buff[i];
    /*check header:*/

    idx=IDX;
    for(i=idx+7;i>idx;i--)
    {
        tailer=tailer+buff[i];
        tailer=tailer<<8;
    }
    tailer=tailer+buff[i];
    /*check tailer:*/

    idx=8;
    while(idx < IDX)
    {
        data=0;
        for(i=idx+7;i>idx;i--)
        {
            data=data+buff[i];
            data=data<<8;
        }
        /*check data of every hit:*/

    }




}


int main(int argc,char *argv[])
{


   char buff[SIZE];
   int data_len;

   int sin_len;
   int socket_descriptor;
   struct sockaddr_in sin,cliaddr;
   bzero(&sin,sizeof(sin));
   sin.sin_family=AF_INET;
   sin.sin_addr.s_addr=inet_addr("127.0.0.1");
   sin.sin_port=htons(58914);
   sin_len=sizeof(sin);
   socket_descriptor=socket(AF_INET,SOCK_DGRAM,0);
   bind(socket_descriptor,(struct sockaddr *)&sin,sizeof(sin));


   while(err_count<64)
   {
     data_len=recvfrom(socket_descriptor,buff,SIZE,0,(struct sockaddr *)&cliaddr,&sin_len);


   }
   printf("data found missed and exit.\n");
   close(socket_descriptor);
}
