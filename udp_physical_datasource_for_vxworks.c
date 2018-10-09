#include <stdio.h>  
#include <stdlib.h>  
#include <string.h>  
#include <sys/socket.h>  
#include <netinet/in.h>  
#include <arpa/inet.h>  
#include <netdb.h>   

const int N=10;
const int M=26;
const int BUFFSIZE=104;
typedef struct {
    /*Header*/
    unsigned int STATUS_BITS;/*is 0*/
    unsigned int LOCAL_L1_COUNT; /*Increment by 1*/
    unsigned int HIT_COUNT;
    unsigned int Timestamp;
    /*data*/
    unsigned int TIGER_ID;
    unsigned int LAST_TIGER_FRAME_NUMBER;
    long long RAW_DATA;
    /*Tailer*/
    unsigned int LOCAL_L1_FRAMENUM;
    unsigned int GEMROC_ID;
    unsigned int CH_ID;
    unsigned int LAST_COUNT_WORD_FROM_TIGER_DATA;
    /*UDP Sequence Counter*/
    long long UDP_packet_count;
}para;

void change_para(para *p,unsigned int*data,int datalen,unsigned char*buff,int buflen)
{
    /*һ֡µıäÉÖ£¬¿ÉԵÝöä¶¼ÔÕÀ£¬ÆËµÄں¯ÊÍÉÖ¡£*/
    int i,tmp;
    for(i=0;i<datalen;i++)
        data[i]=0;

    data[0]=(0x6<<29)+((p->STATUS_BITS & 0x7)<<26)+(p->LOCAL_L1_COUNT>>6);
    data[1]=((p->LOCAL_L1_COUNT & 0x3F)<<24) + ((p->HIT_COUNT & 0xff) <<16) +(p->Timestamp & 0xffff);

    p->Timestamp++;
    for(i=2;i<2*(p->HIT_COUNT)+1;i=i+2)
    {
        data[i]=((p->TIGER_ID & 0x7)<<27 )+( (p->LAST_TIGER_FRAME_NUMBER&0x7) <<24)+((p->RAW_DATA>>30) & 0xffffff);
        data[i+1]=p->RAW_DATA & 0x3fffffff;

        p->RAW_DATA++;
    }
    p->LAST_TIGER_FRAME_NUMBER++;

    /*tailer*/
    i=datalen-4;
    data[i]=(0x7<<29)+((p->LOCAL_L1_FRAMENUM &0xffffff)<<5) + (p->GEMROC_ID&0x1f);
    p->LOCAL_L1_FRAMENUM++;
    i++;
    data[i]=((p->TIGER_ID &0x7)<<27)+ ((p->LOCAL_L1_COUNT&0x7)<<24)+((p->CH_ID&0x3f)<<18)+ (p->LAST_COUNT_WORD_FROM_TIGER_DATA&0x3ffff);
    p->LAST_COUNT_WORD_FROM_TIGER_DATA++;
    p->LOCAL_L1_COUNT++;
    i++;
    data[i]=(0x1<<30) + ((p->GEMROC_ID&0x1f)<<20)+ ((p->UDP_packet_count>>28)&0xfffff);
    i++;
    data[i]=(0x1<<30) + (p->UDP_packet_count & 0xfffffff);
    p->UDP_packet_count++;
    memcpy(buff,(char*)data,buflen);
    /*LinuxÊ¸ß»Ô¸ߵØ·£¬µØ»Ôµ͵Ø·*/
    for(i=0;i<buflen;i=i+4)
    {
        tmp=buff[i];
        buff[i]=buff[i+3];
        buff[i+3]=tmp;
        tmp=buff[i+1];
        buff[i+1]=buff[i+2];
        buff[i+2]=tmp;
    }
}
  
int main(int argc, char** argv) 
{
  
    int socket_descriptor;
    struct sockaddr_in address;

	para PARA;
    unsigned int data[M];
    unsigned char buff[BUFFSIZE];


    PARA.STATUS_BITS=2;     /*²»±ä³õ¯*/
    PARA.LOCAL_L1_COUNT=0;  /*µÝöõ¯*/
    PARA.HIT_COUNT=N;       /*²»±ä³õ¯*/
    PARA.Timestamp=0;       /*µÝöõ¯*/

    PARA.TIGER_ID=2;        /*²»±ä³õ¯*/
    PARA.LAST_TIGER_FRAME_NUMBER=0;  /*µÝöõ¯*/
    PARA.RAW_DATA=0x123456876543;    /*µÝöõ¯*/

    PARA.LOCAL_L1_FRAMENUM=0;   /*µÝöõ¯*/
    PARA.GEMROC_ID=3;      /*²»±ä³õ¯*/
    PARA.CH_ID=8;          /*²»±ä³õ¯*/
    PARA.LAST_COUNT_WORD_FROM_TIGER_DATA=0x5678;  /*µÝöõ¯*/
    PARA.UDP_packet_count=0;



    bzero(&address,sizeof(address));  
    address.sin_family=AF_INET; 
    printf("Input para =0 ,send to 127.0.0.1,else to vxWorks.\n");
    if(argc==2 && atoi(argv[1])==0)
        address.sin_addr.s_addr=inet_addr("127.0.0.1");
    else
        address.sin_addr.s_addr=inet_addr("192.168.1.201");
 
    address.sin_port=htons(58914);  
    socket_descriptor=socket(AF_INET,SOCK_DGRAM,0);
    
    while(1)  
    {
	  change_para(&PARA,data,M,buff,BUFFSIZE);
      sendto(socket_descriptor,buff,sizeof(buff),0,(struct sockaddr *)&address,sizeof(address));
      usleep(100);
    }   
    close(socket_descriptor);  
    printf("Messages Sent\n");    
}  

