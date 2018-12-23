#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/time.h>

#define eventNo 1024
const int maxPacketLen=2096;
const int eventBUFFSIZE=66816;
const int rocBUFFSIZE=2128;
const int rocID_List[33]={7,1,4,30,13,20,26,31};
int       rocID_enable[32];
unsigned int rocFLAG;
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

typedef struct  
{
    unsigned int trigNo;
    unsigned int flag;  
    unsigned char**rocBuff;
}UDPBUFF_INFO;


UDPBUFF_INFO udpInfo[eventNo];

int rocBuff_init() 
{
    int i,j;
    for(i=0;i<32;i++)
        rocID_enable[i]=0;

    if(rocID_List[0]>=32)
    {
        for(i=0;i<32;i++)
            rocID_enable[i]=1;
        rocFLAG=0xffffffff;
    }
    else
    {
        rocFLAG=0;
        for(i=1;i<=rocID_List[0];i++)
        {
            rocID_enable[rocID_List[i]-1]=1;
            rocFLAG=rocFLAG|(0x1<<rocID_List[i]-1);
        }
        j=0;
        for(i=0;i<32;i++)
            if(rocID_enable[i])
                j++;
        if(j!=rocID_List[0])
        {
            printf("Error,number of ROC enabled is not right.\n");
            return -1;
        }
    }
    for(i=0;i<eventNo;i++)
    {
        udpInfo[i].rocBuff=(unsigned char**)malloc(sizeof(unsigned char*)*32);
        if (udpInfo[i].rocBuff == NULL)
        {
            printf("Couldn't allocate memory for udpInfo[%d].rocBuff !\n",i);
            return -1;
        }
        for(j=0;j<32;j++)
        {
            udpInfo[i].rocBuff[j]=NULL;
            if(rocID_enable[j])
                udpInfo[i].rocBuff[j]=(unsigned char*)malloc(maxPacketLen);
            else
                udpInfo[i].rocBuff[j]=(unsigned char*)malloc(4);
            if (udpInfo[i].rocBuff[j] == NULL)
            {
                printf("Couldn't allocate memory for udpInfo[%d].rocBuff[%d] !\n",i,j);
                return -1;
            }
            *(unsigned int*)udpInfo[i].rocBuff[j]=0;
        }
        udpInfo[i].trigNo=0;
        udpInfo[i].flag=rocFLAG;
    }
    printf("rocFLAG=%08X\n",rocFLAG);
    printf("Receive rocID enable:\n");
    for(i=0;i<32;i++)
    {
        printf("rocID %d : ",i);
        if(rocID_enable[i])
            printf("yes\n");
        else
            printf("no\n");
    }
    for(i=0;i<32;i++)
        if(rocID_enable[i])
            printf("ROC %d is receive enable.\n",i);

    return 0;
}

int copy_to_rocBuff(unsigned char*udpRecvBuff,unsigned int buffLen,
                unsigned int  trgNo,unsigned int  rocID)
{
    if(rocID_enable[rocID]==0)
    {
     /*   printf("Recv an extra packet,triggerID=%d, rocID=%d\n",trgNo,rocID); */
        return 0;
    }
    unsigned int idx=trgNo%eventNo;
    int i;
    if(trgNo==udpInfo[idx].trigNo)
    {
        if((udpInfo[idx].flag>>rocID)&0x1)
        {
            *(unsigned int*)udpInfo[idx].rocBuff[rocID] =buffLen;
            memcpy(udpInfo[idx].rocBuff[rocID]+4,udpRecvBuff,buffLen);

            udpInfo[idx].flag=udpInfo[idx].flag&(0xffffffff^(0x1<<rocID));
            if(udpInfo[idx].flag==0)  /*·¢ËÍÕbuffºóØÃlag*/
                return 1;
            else
                return 0;
        }
        else  /*if receive a duplicate packet*/
        {
            printf("Error! Receive a duplicate packet,TrigerID=%d, rocID=%d .\n",trgNo,rocID);
            printf("packet 1, length= %d: ,",*(unsigned int*)udpInfo[idx].rocBuff[rocID]);
            for(i=0;i<*(unsigned int*)udpInfo[idx].rocBuff[rocID] && i<32;i++)
            {
                if(i&0x11==0)
                    printf(" ");
                printf("%02X",udpInfo[idx].rocBuff[rocID][i+4]);
            }
            printf("\npacket 2, length= %d: ,",buffLen);
            for(i=0;i<buffLen && i<32;i++)
            {
                if(i&0x11==0)
                    printf(" ");
                printf("%02X",udpInfo[idx].rocBuff[rocID][i]);
            }
			printf("\n");
            return 0;
        }
    }
    else if(trgNo>udpInfo[idx].trigNo)
    {
        if(udpInfo[idx].flag!=rocFLAG)
            printf("Found a udp packet missed,its triggerNo=%d.\n",udpInfo[idx].trigNo);
        *(unsigned int*)udpInfo[idx].rocBuff[rocID] =buffLen;
        memcpy(udpInfo[idx].rocBuff[rocID]+4,udpRecvBuff,buffLen);
        udpInfo[idx].flag=rocFLAG&(0xffffffff^(0x1<<rocID)); /*¸ügÓЧ±ê*/
        udpInfo[idx].trigNo=trgNo;
        if(udpInfo[idx].flag==0) 
            return 1;
        else
            return 0;
    }
    else
    {
        printf("Recv a expired udp packet,triggerNo packet=%d, rocID=%d.\n",trgNo,rocID);
        return 0;
    }
}
void copy_to_sendBuff(unsigned int trgID,unsigned char*outputBuff)
{
    trgID=trgID%eventNo;
   unsigned int sndLen=0;
   int i,j;
   int idx=4;
   unsigned int rocTmplen;
   for(i=0;i<32;i++)
   if(rocID_enable[i])
   {
       rocTmplen=*(unsigned int*)udpInfo[trgID].rocBuff[i];
       memcpy(outputBuff+idx,udpInfo[trgID].rocBuff[i]+4,rocTmplen);
       idx=idx+rocTmplen;
       sndLen=sndLen+rocTmplen;
   }
   *(unsigned int*)outputBuff=sndLen;
   udpInfo[trgID].flag=rocFLAG;
}

void rocBuff_delete()
{
    int i,j;
    for(i=0;i<eventNo;i++)
    for(j=0;j<32;j++)
        if (udpInfo[i].rocBuff[j])
        {
            free(udpInfo[i].rocBuff[j]);
            udpInfo[i].rocBuff[j]=NULL;
        }
    free(udpInfo[i].rocBuff);
}

int extract_or_print_udp_para(unsigned char*buff,unsigned int buflen,para* p,int print_enable)
{
    unsigned int tmp_idx;
    unsigned int i;

    if(buflen<24)
    {
        printf("buff length =%d,  error .\n",buflen);
        return -1;
    }
    if((buff[0]>>5)!=6)
    {
        printf("header_flag= %d, error .\n",buff[0]>>5);
        return -1;
    }
    p->STATUS_BITS=(buff[0]>>2)&0x7;

    p->LOCAL_L1_COUNT=buff[0]&0x3;
    p->LOCAL_L1_COUNT=p->LOCAL_L1_COUNT<<8;
    p->LOCAL_L1_COUNT=p->LOCAL_L1_COUNT+buff[1];
    p->LOCAL_L1_COUNT=p->LOCAL_L1_COUNT<<8;
    p->LOCAL_L1_COUNT=p->LOCAL_L1_COUNT+buff[2];
    p->LOCAL_L1_COUNT=p->LOCAL_L1_COUNT<<8;
    p->LOCAL_L1_COUNT=p->LOCAL_L1_COUNT+buff[3];
    p->LOCAL_L1_COUNT=p->LOCAL_L1_COUNT<<6;
    p->LOCAL_L1_COUNT=p->LOCAL_L1_COUNT+(buff[4]&0x3f);

    p->HIT_COUNT=buff[5];

    p->Timestamp=buff[6];
    p->Timestamp=p->Timestamp<<8;
    p->Timestamp=p->Timestamp+buff[7];

    if((p->HIT_COUNT<<3)+24 != buflen)
    {
        printf("HIT_COUNT= %d, recv buff length error .\n",p->HIT_COUNT);
        return -1;
    }

    /* tailer part */
    tmp_idx=(p->HIT_COUNT<<3)+8;

    if((buff[tmp_idx]>>5)!=7)
    {
        printf("tailer_flag= %d, error .\n",buff[tmp_idx]>>5);
        return -1;
    }

    p->LOCAL_L1_FRAMENUM=buff[tmp_idx]&0x1f;
    p->LOCAL_L1_FRAMENUM=p->LOCAL_L1_FRAMENUM<<8;
    tmp_idx++;
    p->LOCAL_L1_FRAMENUM=p->LOCAL_L1_FRAMENUM+buff[tmp_idx];
    p->LOCAL_L1_FRAMENUM=p->LOCAL_L1_FRAMENUM<<8;
    tmp_idx++;
    p->LOCAL_L1_FRAMENUM=p->LOCAL_L1_FRAMENUM+buff[tmp_idx];
    p->LOCAL_L1_FRAMENUM=p->LOCAL_L1_FRAMENUM<<3;
    tmp_idx++;
    p->LOCAL_L1_FRAMENUM=p->LOCAL_L1_FRAMENUM+(buff[tmp_idx]>>5);
    p->GEMROC_ID=buff[tmp_idx]&0x1f;

    tmp_idx++;
    p->T_TIGER_ID=(buff[tmp_idx]>>3)&0x7;
    if((buff[tmp_idx] & 0x7) != (p->LOCAL_L1_COUNT & 0x7) )
    {
        printf("buff[tmp_idx]&0x7= %d , ",buff[tmp_idx] & 0x7);
        printf("LOCAL_L1_COUNT & 0x7= %d , error.\n",p->LOCAL_L1_COUNT & 0x7);
        return -1;
    }
    tmp_idx++;
    p->CH_ID=buff[tmp_idx]>>2;

    p->LAST_COUNT_WORD_FROM_TIGER_DATA=buff[tmp_idx]&0x3;
    p->LAST_COUNT_WORD_FROM_TIGER_DATA=p->LAST_COUNT_WORD_FROM_TIGER_DATA<<8;
    tmp_idx++;
    p->LAST_COUNT_WORD_FROM_TIGER_DATA=p->LAST_COUNT_WORD_FROM_TIGER_DATA + buff[tmp_idx];
    p->LAST_COUNT_WORD_FROM_TIGER_DATA=p->LAST_COUNT_WORD_FROM_TIGER_DATA<<8;
    tmp_idx++;
    p->LAST_COUNT_WORD_FROM_TIGER_DATA=p->LAST_COUNT_WORD_FROM_TIGER_DATA + buff[tmp_idx];
    /*UDP Sequence Counter part*/
    tmp_idx++;

    if((buff[tmp_idx]>>4)!=4)
    {
        printf(" sequence_falg1 = %d, sequence_falg1!=4 ,error.\n",buff[tmp_idx]>>4);
        return -1;
    }


    p->S_GEMROC_ID=buff[tmp_idx]&0x1;
    p->S_GEMROC_ID=p->S_GEMROC_ID<<4;
    tmp_idx++;
    p->S_GEMROC_ID=p->S_GEMROC_ID+(buff[tmp_idx]>>4);

    p->UDP_packet_count=buff[tmp_idx]&0xf;
    p->UDP_packet_count=p->UDP_packet_count<<8;
    tmp_idx++;
    p->UDP_packet_count=p->UDP_packet_count+buff[tmp_idx];
    p->UDP_packet_count=p->UDP_packet_count<<8;
    tmp_idx++;
    p->UDP_packet_count=p->UDP_packet_count+buff[tmp_idx];
    tmp_idx++;

    if((buff[tmp_idx]>>4)!=4)
    {
        printf(" sequence_falg2 = %d, sequence_falg2!=4 ,error.\n",buff[tmp_idx]>>4);
        return -1;
    }

    p->UDP_packet_count=p->UDP_packet_count<<4;
    p->UDP_packet_count=p->UDP_packet_count+(buff[tmp_idx]&0xf);
    p->UDP_packet_count=p->UDP_packet_count<<8;
    tmp_idx++;
    p->UDP_packet_count=p->UDP_packet_count+buff[tmp_idx];
    p->UDP_packet_count=p->UDP_packet_count<<8;
    tmp_idx++;
    p->UDP_packet_count=p->UDP_packet_count+buff[tmp_idx];
    p->UDP_packet_count=p->UDP_packet_count<<8;
    tmp_idx++;
    p->UDP_packet_count=p->UDP_packet_count+buff[tmp_idx];

    if(print_enable)
    {
        printf("Header:\n");
        printf( "STATUS_BITS= %01X,  LOCAL_L1_COUNT= %u,  HIT_COUNT= %d,   Timestamp= %04X\n",
               p->STATUS_BITS,p->LOCAL_L1_COUNT,p->HIT_COUNT,p->Timestamp);
        printf("Data:\n");
        for(i=0,tmp_idx=8; i<p->HIT_COUNT; i++)
        {
            p->D_TIGER_ID=(buff[tmp_idx]>>3)&0x7;
            p->LAST_TIGER_FRAME_NUMBER=buff[tmp_idx]&0x7;
            tmp_idx++;
            p->RAW_DATA=buff[tmp_idx];
            tmp_idx++;
            p->RAW_DATA=p->RAW_DATA<<8;
            p->RAW_DATA=p->RAW_DATA+buff[tmp_idx];
            tmp_idx++;
            p->RAW_DATA=p->RAW_DATA<<8;
            p->RAW_DATA=p->RAW_DATA+buff[tmp_idx];

            tmp_idx++;
            p->RAW_DATA=p->RAW_DATA<<6;
            p->RAW_DATA=p->RAW_DATA+(buff[tmp_idx]&0x3f);
            tmp_idx++;
            p->RAW_DATA=p->RAW_DATA<<8;
            p->RAW_DATA=p->RAW_DATA+buff[tmp_idx];
            tmp_idx++;
            p->RAW_DATA=p->RAW_DATA<<8;
            p->RAW_DATA=p->RAW_DATA+buff[tmp_idx];
            tmp_idx++;
            p->RAW_DATA=p->RAW_DATA<<8;
            p->RAW_DATA=p->RAW_DATA+buff[tmp_idx];
            tmp_idx++;
            printf("D_TIGER_ID= %01X,  LAST_TIGER_FRAME_NUMBER= %01X,   RAW_DATA= %06X" ,
                   p->D_TIGER_ID,p->LAST_TIGER_FRAME_NUMBER,(p->RAW_DATA>>32)&0x3fffff);
            printf("%08X\n" ,p->RAW_DATA&0xffffffff);
        }
        printf("Tailer:\n");
        printf("LOCAL_L1_FRAMENUM=%06X, GEMROC_ID=%02d, T_TIGER_ID=%01X,  CH_ID= %02X,  LAST_COUNT_WORD_FROM_TIGER_DATA=%05X\n",
               p->LOCAL_L1_FRAMENUM,p->GEMROC_ID,p->T_TIGER_ID,p->CH_ID,p->LAST_COUNT_WORD_FROM_TIGER_DATA);
        printf("UDP Sequence Counter:\n");
        printf("S_GEMROC_ID= %02X, UDP_packet_count= %04X",p->S_GEMROC_ID,p->UDP_packet_count>>32);
        printf("%08X\n",p->UDP_packet_count&0xffffffff);
        printf("\n");
    }
    return 0;
}

int main(int argc, char** argv)
{

	unsigned char eventBuff[eventBUFFSIZE];  
	unsigned char rocBuff[rocBUFFSIZE];
	int tmp;
	para tmp_para;

   int port=58914;
   int sin_len;
   int socket_descriptor;
	int recv_len;

   struct sockaddr_in sin,cliaddr;
   bzero(&sin,sizeof(sin));
   sin.sin_family=AF_INET;
   sin.sin_addr.s_addr=htonl(INADDR_ANY);

   sin.sin_port=htons(port);
   sin_len=sizeof(sin);
   socket_descriptor=socket(AF_INET,SOCK_DGRAM,0);
   bind(socket_descriptor,(struct sockaddr *)&sin,sizeof(sin));

    int udpLoop;
	int a,b,trg;
    rocBuff_init();
	b=0;
	unsigned int nCount=0;
   while(1)
   {
		udpLoop=1;
		while(udpLoop)
		{
			
       		recv_len=recvfrom(socket_descriptor,rocBuff,rocBUFFSIZE,0,(struct sockaddr *)&cliaddr,&sin_len);
			extract_or_print_udp_para(rocBuff,recv_len,&tmp_para,0);
			tmp=copy_to_rocBuff(rocBuff,recv_len,tmp_para.LOCAL_L1_COUNT,tmp_para.GEMROC_ID);
			if(tmp==1)
				udpLoop=0;
		}
		copy_to_sendBuff(tmp_para.LOCAL_L1_COUNT,eventBuff);
		trg=tmp_para.LOCAL_L1_COUNT % eventNo;
        for(a=0;a<32;a++)
        {
			if(rocID_enable[a]);
            //	extract_or_print_udp_para(udpInfo[trg].rocBuff[a]+4,*(unsigned int*)udpInfo[trg].rocBuff[a],&tmp_para,1);
        }
	nCount++;
	if(nCount%100==0)
		printf("Event sent,triggerID = %d\n",tmp_para.LOCAL_L1_COUNT);
       
   }

	rocBuff_delete();
    printf("  finish.\n");
  close(socket_descriptor);
}

















