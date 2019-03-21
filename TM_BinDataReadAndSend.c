#include <stdio.h>  
#include <stdlib.h>  
#include <string.h>  
#include <sys/socket.h>  
#include <netinet/in.h>  
#include <arpa/inet.h>  
#include <netdb.h>   

const int BUFFSIZE=25600;

  
int main(int argc, char** argv) 
{
  
    int socket_descriptor;
    struct sockaddr_in address;

    unsigned char buff[BUFFSIZE];
    FILE *fp = NULL;
    fp = fopen("cgem_TM.bin", "rb");
    int Total_Data=0;
    Total_Data=fread(buff,sizeof(unsigned char),BUFFSIZE,fp);
    fclose(fp);
    int p1,p2;
     int i;
    p1=7;
    while((buff[p1]>>5)!=6 && p1<Total_Data)
	p1=p1+8;
    p1=p1-7;
    printf(" %d  Bytes data read from file.\n",Total_Data);
    p2=Total_Data-Total_Data%8-1;
    while((buff[p2]>>4)!=4 && p2>0)
        p2=p2-8;
    p2=p2+1;
    Total_Data=p2-p1;
    printf("buff[%d]=0x%02X,buff[%d]=0x%02X\n",p1+7,buff[p1+7],p2-1,buff[p2-1]);
    if(p1)
    for(i=0;i<Total_Data;i++)
        buff[i]=buff[p1+i];
    printf("Read %d  Bytes from TM_bin file.\n",Total_Data);

    bzero(&address,sizeof(address));  
    address.sin_family=AF_INET; 
    address.sin_addr.s_addr=inet_addr("127.0.0.1");
    address.sin_port=htons(58880);  
    socket_descriptor=socket(AF_INET,SOCK_DGRAM,0);
    int EventLen=0;
    i=0;
    while(1)  
    {
        if(i<Total_Data)
        {
		EventLen=buff[i+2]*8+24;
		sendto(socket_descriptor,buff+i,EventLen,0,(struct sockaddr *)&address,sizeof(address));
		i=i+EventLen;
	}
	else
		i=0;
        usleep(1300);
    }

    close(socket_descriptor);  
    printf("Messages Sent\n");    
}  

