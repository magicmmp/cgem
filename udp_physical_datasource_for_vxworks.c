#include <stdio.h>  
#include <stdlib.h>  
#include <string.h>  
#include <sys/socket.h>  
#include <netinet/in.h>  
#include <arpa/inet.h>  
#include <netdb.h>   

/** hit count=10 **/
/** need 106 of int length **/
#define N     2  /*HIT_COUNT*/
#define M     ((N<<1)+6)
#define rocNo 32

const int BUFFSIZE=M<<2;

typedef struct {
    /*Header*/
    unsigned int STATUS_BITS;/*is 0*/
    unsigned int LOCAL_L1_COUNT; /*Increment by 1*/
    unsigned int HIT_COUNT;
    unsigned int Timestamp;
    /*1¸öµÄata*/
    unsigned int D_TIGER_ID;
    unsigned int LAST_TIGER_FRAME_NUMBER;
    long long RAW_DATA;
    /*Tailer*/
    unsigned int LOCAL_L1_FRAMENUM;
    unsigned int T_TIGER_ID; /*tailer TIGER_ID*/
    unsigned int GEMROC_ID;
    unsigned int CH_ID;
    unsigned int LAST_COUNT_WORD_FROM_TIGER_DATA;
    /*UDP Sequence Counter*/
    long long UDP_packet_count;
    unsigned int S_GEMROC_ID;
}para;

void change_para(para *p,unsigned int*data,int datalen,unsigned char*buff,int buflen)
{

    int i,tmp;
    int Va;  /*test if is little end*/
    for(i=0;i<datalen;i++)
        data[i]=0;

    data[0]=(0x6<<29)+((p->STATUS_BITS & 0x7)<<26)+(p->LOCAL_L1_COUNT>>6);
    data[1]=((p->LOCAL_L1_COUNT & 0x3F)<<24) + ((p->HIT_COUNT & 0xff) <<16) +(p->Timestamp & 0xffff);

    p->Timestamp++;
    for(i=2;i<2*(p->HIT_COUNT)+1;i=i+2)
    {
        p->RAW_DATA++;
        data[i]=((p->D_TIGER_ID & 0x7)<<27 )+( (p->LAST_TIGER_FRAME_NUMBER&0x7) <<24)+((p->RAW_DATA>>30) & 0xffffff);
        data[i+1]=p->RAW_DATA & 0x3fffffff;
    }
    p->LAST_TIGER_FRAME_NUMBER++;

    /*tailer*/
    i=datalen-4;
    data[i]=(0x7<<29)+((p->LOCAL_L1_FRAMENUM &0xffffff)<<5) + (p->GEMROC_ID&0x1f);
    p->LOCAL_L1_FRAMENUM++;
    i++;
    data[i]=((p->D_TIGER_ID &0x7)<<27)+ ((p->LOCAL_L1_COUNT&0x7)<<24)+((p->CH_ID&0x3f)<<18)+ (p->LAST_COUNT_WORD_FROM_TIGER_DATA&0x3ffff);
    p->LAST_COUNT_WORD_FROM_TIGER_DATA++;

    /*sequence*/
    i++;
    data[i]=(0x1<<30) + ((p->GEMROC_ID&0x1f)<<20)+ ((p->UDP_packet_count>>28)&0xfffff);
    i++;
    data[i]=(0x1<<30) + (p->UDP_packet_count & 0xfffffff);
    p->UDP_packet_count++;
    memcpy(buff,(char*)data,buflen);
    Va=1;
    if (*(char*)&Va == 1)
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
	int x,y;

	PARA.STATUS_BITS=2;     /*²»±ä³õ¯*/
    PARA.LOCAL_L1_COUNT=0;  /*µÝöõ¯*/
    PARA.HIT_COUNT=N;       /*²»±ä³õ¯*/
    PARA.Timestamp=0;       /*µÝöõ¯*/

    PARA.D_TIGER_ID=2;        /*²»±ä³õ¯*/
    PARA.LAST_TIGER_FRAME_NUMBER=0;  /*µÝöõ¯*/
    PARA.RAW_DATA=0x123456876543;    /*µÝöõ¯*/

    PARA.LOCAL_L1_FRAMENUM=0;   /*µÝöõ¯*/
    PARA.GEMROC_ID=3;      /*²»±ä³õ¯*/
    PARA.CH_ID=8;          /*²»±ä³õ¯*/
    PARA.LAST_COUNT_WORD_FROM_TIGER_DATA=0x5678;  /*µÝöõ¯*/
    PARA.UDP_packet_count=0;


    bzero(&address,sizeof(address));  
    address.sin_family=AF_INET; 
    printf("Input para =0 ,send to 127.0.0.1,para=1,send to baiduYun,else to vxWorks.\n");
    if(argc==2 && atoi(argv[1])==0)
        address.sin_addr.s_addr=inet_addr("127.0.0.1");
    else if(argc==2 && atoi(argv[1])==1)
        address.sin_addr.s_addr=inet_addr("182.61.26.154");
    else
	address.sin_addr.s_addr=inet_addr("192.168.1.201");
 
    address.sin_port=htons(58914);  
    socket_descriptor=socket(AF_INET,SOCK_DGRAM,0);
    
    unsigned char tmp;
    int i,j;
    unsigned int *p;
	printf("gemroc_Numbers = %d, HIT_COUNT = %d\n",rocNo,N);
	printf("BUFFSIZE    = %d\n",BUFFSIZE);
    

    while(1)  
    {

	for(i=0;i<rocNo;i++)
	{
		PARA.GEMROC_ID=i;
		change_para(&PARA,data,M,buff,BUFFSIZE);
		sendto(socket_descriptor,buff,sizeof(buff),0,(struct sockaddr *)&address,sizeof(address));
	}
	PARA.LOCAL_L1_COUNT++;
	
        usleep(1000);
    }
    close(socket_descriptor);  
    printf("Messages Sent\n");    
}  

