#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/time.h>

const int  BUFFSIZE=2096;

int data_check(unsigned char*buff,unsigned int buflen,unsigned int* PRE_COUNT,int print_enable)
{
    /* error return -1,else return 0*/
    /*Header*/
    unsigned int header_flag;
    unsigned int STATUS_BITS;    /*is 0*/
    unsigned int LOCAL_L1_COUNT; /*Increment by 1*/
    unsigned int HIT_COUNT;
    unsigned int Timestamp;
    /*data*/
    unsigned int D_TIGER_ID;  /*TIGER_ID*/
    unsigned int LAST_TIGER_FRAME_NUMBER;
    long long RAW_DATA;
    /*Tailer*/
    unsigned int tailer_flag;
    unsigned int LOCAL_L1_FRAMENUM;
    unsigned int GEMROC_ID;
    unsigned int T_TIGER_ID; /*tailer TIGER_ID*/
    unsigned int CH_ID;
    unsigned int LAST_COUNT_WORD_FROM_TIGER_DATA;
    /*UDP Sequence Counter*/
    unsigned int sequence_falg1;
    unsigned int sequence_falg2;
    unsigned int S_GEMROC_ID;
    long long UDP_packet_count;

    unsigned int tmp_idx;
    unsigned int i;
    /*data length error*/
    if(buflen<24)
    {
        printf("buff length =%d,  error .\n",buflen);
        /*
 *         return -1;
 *                 */
    }
        /* header part */
    header_flag=buff[0]>>5;
    STATUS_BITS=(buff[0]>>2)&0x7;

    LOCAL_L1_COUNT=buff[0]&0x3;
    LOCAL_L1_COUNT=LOCAL_L1_COUNT<<8;
    LOCAL_L1_COUNT=LOCAL_L1_COUNT+buff[1];
    LOCAL_L1_COUNT=LOCAL_L1_COUNT<<8;
    LOCAL_L1_COUNT=LOCAL_L1_COUNT+buff[2];
    LOCAL_L1_COUNT=LOCAL_L1_COUNT<<8;
    LOCAL_L1_COUNT=LOCAL_L1_COUNT+buff[3];
    LOCAL_L1_COUNT=LOCAL_L1_COUNT<<6;
    LOCAL_L1_COUNT=LOCAL_L1_COUNT+(buff[4]&0x3f);

    HIT_COUNT=buff[5];


    Timestamp=buff[6];
    Timestamp=Timestamp<<8;
    Timestamp=Timestamp+buff[7];
    if(header_flag!=6)
    {
        printf("header_flag= %d, error .\n",header_flag);
        /*
 *         return -1;
 *                 */
    }
    if((HIT_COUNT<<3)+24 != buflen)
    {
        printf("HIT_COUNT= %d, recv buff length error .\n",HIT_COUNT);
        /*
 *         return -1;
 *                 */
    }

    if(*PRE_COUNT+1 != LOCAL_L1_COUNT)
    {
        printf("*PRE_COUNT= %u, LOCAL_L1_COUNT= %u, error .\n",*PRE_COUNT,LOCAL_L1_COUNT);
        /*
 *         return -1;
 *                 */
    }
    *PRE_COUNT= LOCAL_L1_COUNT;
    /* tailer part */
    tmp_idx=(HIT_COUNT<<3)+8;
    tailer_flag=buff[tmp_idx]>>5;

    LOCAL_L1_FRAMENUM=buff[tmp_idx]&0x1f;
    LOCAL_L1_FRAMENUM=LOCAL_L1_FRAMENUM<<8;
    tmp_idx++;
    LOCAL_L1_FRAMENUM=LOCAL_L1_FRAMENUM+buff[tmp_idx];
    LOCAL_L1_FRAMENUM=LOCAL_L1_FRAMENUM<<8;
    tmp_idx++;
    LOCAL_L1_FRAMENUM=LOCAL_L1_FRAMENUM+buff[tmp_idx];
    LOCAL_L1_FRAMENUM=LOCAL_L1_FRAMENUM<<3;
    tmp_idx++;
    LOCAL_L1_FRAMENUM=LOCAL_L1_FRAMENUM+(buff[tmp_idx]>>5);
    GEMROC_ID=buff[tmp_idx]&0x1f;

    tmp_idx++;
    T_TIGER_ID=(buff[tmp_idx]>>3)&0x7;
    if((buff[tmp_idx] & 0x7) != (LOCAL_L1_COUNT & 0x7) )
    {
        printf("buff[tmp_idx]&0x7= %d , ",buff[tmp_idx] & 0x7);
        printf("LOCAL_L1_COUNT & 0x7= %d , error.\n",LOCAL_L1_COUNT & 0x7);
        /*
 *         return -1;
 *                 */
    }
    tmp_idx++;
    CH_ID=buff[tmp_idx]>>2;

    LAST_COUNT_WORD_FROM_TIGER_DATA=buff[tmp_idx]&0x3;
    LAST_COUNT_WORD_FROM_TIGER_DATA=LAST_COUNT_WORD_FROM_TIGER_DATA<<8;
    tmp_idx++;
    LAST_COUNT_WORD_FROM_TIGER_DATA=LAST_COUNT_WORD_FROM_TIGER_DATA + buff[tmp_idx];
    LAST_COUNT_WORD_FROM_TIGER_DATA=LAST_COUNT_WORD_FROM_TIGER_DATA<<8;
    tmp_idx++;
    LAST_COUNT_WORD_FROM_TIGER_DATA=LAST_COUNT_WORD_FROM_TIGER_DATA + buff[tmp_idx];
    /*UDP Sequence Counter part*/
    tmp_idx++;
    sequence_falg1=buff[tmp_idx]>>4;

    S_GEMROC_ID=buff[tmp_idx]&0x1;
    S_GEMROC_ID=S_GEMROC_ID<<4;
    tmp_idx++;
    S_GEMROC_ID=S_GEMROC_ID+(buff[tmp_idx]>>4);

    UDP_packet_count=buff[tmp_idx]&0xf;
    UDP_packet_count=UDP_packet_count<<8;
    tmp_idx++;
    UDP_packet_count=UDP_packet_count+buff[tmp_idx];
    UDP_packet_count=UDP_packet_count<<8;
    tmp_idx++;
    UDP_packet_count=UDP_packet_count+buff[tmp_idx];
    tmp_idx++;

    sequence_falg2=buff[tmp_idx]>>4;

    UDP_packet_count=UDP_packet_count<<4;
    UDP_packet_count=UDP_packet_count+(buff[tmp_idx]&0xf);
    UDP_packet_count=UDP_packet_count<<8;
    tmp_idx++;
    UDP_packet_count=UDP_packet_count+buff[tmp_idx];
    UDP_packet_count=UDP_packet_count<<8;
    tmp_idx++;
    UDP_packet_count=UDP_packet_count+buff[tmp_idx];
    UDP_packet_count=UDP_packet_count<<8;
    tmp_idx++;
    UDP_packet_count=UDP_packet_count+buff[tmp_idx];
    if(sequence_falg1!=4)
    {
        printf(" sequence_falg1 = %d, sequence_falg1!=4 ,error.\n",sequence_falg1);
        /*
 *         return -1;
 *                 */
    }
    if(sequence_falg2!=4)
    {
        printf(" sequence_falg2 = %d, sequence_falg2!=4 ,error.\n",sequence_falg2);
        /*
 *         return -1;
 *                 */
    }

    if(print_enable)
    {
        printf("Header:\n");
        printf( "STATUS_BITS= %01X,  LOCAL_L1_COUNT= %08X,  HIT_COUNT= %d,   Timestamp= %04X\n",STATUS_BITS,LOCAL_L1_COUNT,HIT_COUNT,Timestamp);
        printf("Data:\n");
        for(i=0,tmp_idx=8; i<HIT_COUNT; i++)
        {
            D_TIGER_ID=(buff[tmp_idx]>>3)&0x7;
            LAST_TIGER_FRAME_NUMBER=buff[tmp_idx]&0x7;
            tmp_idx++;
            RAW_DATA=buff[tmp_idx];
            tmp_idx++;
            RAW_DATA=RAW_DATA<<8;
            RAW_DATA=RAW_DATA+buff[tmp_idx];
            tmp_idx++;
            RAW_DATA=RAW_DATA<<8;
            RAW_DATA=RAW_DATA+buff[tmp_idx];

            tmp_idx++;
            RAW_DATA=RAW_DATA<<6;
            RAW_DATA=RAW_DATA+(buff[tmp_idx]&0x3f);
            tmp_idx++;
            RAW_DATA=RAW_DATA<<8;
            RAW_DATA=RAW_DATA+buff[tmp_idx];
            tmp_idx++;
            RAW_DATA=RAW_DATA<<8;
            RAW_DATA=RAW_DATA+buff[tmp_idx];
            tmp_idx++;
            RAW_DATA=RAW_DATA<<8;
            RAW_DATA=RAW_DATA+buff[tmp_idx];
            tmp_idx++;
            printf("D_TIGER_ID= %01X,  LAST_TIGER_FRAME_NUMBER= %01X,   RAW_DATA= %06X" ,D_TIGER_ID,LAST_TIGER_FRAME_NUMBER,(RAW_DATA>>32)&0x3fffff);
            printf("%08X\n" ,RAW_DATA&0xffffffff);
        }
        printf("Tailer:\n");
        printf("LOCAL_L1_FRAMENUM=%06X, GEMROC_ID=%02X, T_TIGER_ID=%01X,  CH_ID= %02X,  LAST_COUNT_WORD_FROM_TIGER_DATA=%05X\n",LOCAL_L1_FRAMENUM,GEMROC_ID,T_TIGER_ID,CH_ID,LAST_COUNT_WORD_FROM_TIGER_DATA);
        printf("UDP Sequence Counter:\n");
        printf("S_GEMROC_ID= %02X, UDP_packet_count= %04X",S_GEMROC_ID,UDP_packet_count>>32);
        printf("%08X\n",UDP_packet_count&0xffffffff);
        printf("\n");
    }


    return 0;
}



int main(int argc, char** argv)
{  
   unsigned char buff[BUFFSIZE];
   int port=58914;
   int sin_len;
   int socket_descriptor;
	int recv_len;
	unsigned int PRE_COUNT=0;
	int print_enable;
   struct sockaddr_in sin,cliaddr;
   bzero(&sin,sizeof(sin));
   sin.sin_family=AF_INET;

    printf("Input para =0 ,bind 127.0.0.1,else bind 192.168.1.200.\n");
    if(argc==2 && atoi(argv[1])==0)
        sin.sin_addr.s_addr=inet_addr("127.0.0.1");
    else
        sin.sin_addr.s_addr=inet_addr("192.168.1.200");

   sin.sin_port=htons(port);
   sin_len=sizeof(sin);
   socket_descriptor=socket(AF_INET,SOCK_DGRAM,0);
   bind(socket_descriptor,(struct sockaddr *)&sin,sizeof(sin));
   print_enable=1;
   while(1)
   {
       recv_len=recvfrom(socket_descriptor,buff,BUFFSIZE,0,(struct sockaddr *)&cliaddr,&sin_len);
       data_check(buff,recv_len,&PRE_COUNT,print_enable);
   }
    printf("  finish.\n");
  close(socket_descriptor);
}
