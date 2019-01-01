#include<stdio.h>
#include<stdlib.h>
#include<netinet/in.h>
#include <sys/types.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<netdb.h>
#include<string.h>
#include <unistd.h>
#include <sys/time.h>
#include <netinet/tcp.h>
#include <errno.h>
#include <pthread.h>
#include <iostream>

using namespace std;

	int tcpLoop;
	int cmd_fd;
    int txt_fd;
    int data_fd;
	char buff[102400];

void* cmd_send(void* args)
{
	send(cmd_fd,"I am cmd send thread.",22,MSG_DONTWAIT);
	sleep(60);
	send(cmd_fd,"stop",4,MSG_DONTWAIT);
	sleep(1);
	tcpLoop=0;
    close(cmd_fd);
	printf("exit cmd send thread.\n");
	
}

void* get_txt(void* args)
{
	unsigned int Len;
    unsigned char tcpBuffHeader[4]; 
	recv(txt_fd,tcpBuffHeader,4,0) ;
    Len=*(unsigned int*)tcpBuffHeader;
    printf("*(unsigned int*)tcpBuffHeader= %X\n",Len);

	send(txt_fd,"I am txt get thread.",21,MSG_DONTWAIT);
    close(txt_fd);
	printf("exit get txt thread.\n");
}

void* dataRecv(void* args)
{
	int len;
	int nPacket=0;
	int tmpLen;
	unsigned int eventLen;
	FILE *fp = NULL;
	fp = fopen("../tcp.bin", "wb");
	*(unsigned int*)buff=0;
	while(tcpLoop)
	{
		len=recv(data_fd,buff,4,0) ;
		if(len>0)
		{
			eventLen=*(unsigned int*)buff;
			*(unsigned int*)buff=0;
			tmpLen=0;
			while(tmpLen<eventLen && tcpLoop)
			{
				len=recv(data_fd,buff+tmpLen+4,eventLen-tmpLen,0) ;
				if(len>0)
				{
					tmpLen=tmpLen+len;
				}
				else if(len==0)
				{
            		tcpLoop=0;
            		printf("tcp socket closed by remote end.\n");
        		}
				else
				{
					printf("tcp socket return len =-1.\n");
				}			
			}

		}
		else if(len==0)
		{
			tcpLoop=0;
			printf("tcp socket closed by remote end.\n");
		}
		else
        {
        	printf("tcp socket return len =-1.\n");
        }
		if(tmpLen==eventLen && tmpLen>20)
		{
			fwrite(buff+4,sizeof(unsigned char),tmpLen,fp);
			nPacket++;
			tmpLen=0;
		}
	}
	fclose(fp);
	close(data_fd);
	printf("%d events have been written to bin-file.\n",nPacket);
	printf("exit data recv thread.\n");
}

int main()
{  
    struct sockaddr_in    servaddr;
    cmd_fd = socket(AF_INET, SOCK_STREAM, 0);
	txt_fd = socket(AF_INET, SOCK_STREAM, 0);
	data_fd = socket(AF_INET, SOCK_STREAM, 0);

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(56898);
    inet_pton(AF_INET,"127.0.0.1", &servaddr.sin_addr);
    tcpLoop=1;
    connect(cmd_fd, (struct sockaddr*)&servaddr, sizeof(servaddr));
	connect(txt_fd, (struct sockaddr*)&servaddr, sizeof(servaddr));
	connect(data_fd, (struct sockaddr*)&servaddr, sizeof(servaddr));

    	pthread_t cmd_tid;
        pthread_t txt_tid;
        pthread_t data_tid;

        int ret = pthread_create(&txt_tid, NULL, get_txt, NULL);
        if (ret != 0)
        {
           cout << "pthread get_txt() create error: error_code=" << ret << endl;
           exit(0);
        }
        ret = pthread_create(&cmd_tid, NULL, cmd_send, NULL);
        if (ret != 0)
        {
           cout << "pthread cmd_send() create error: error_code=" << ret << endl;
           exit(0);
        }
        ret = pthread_create(&data_tid, NULL, dataRecv, NULL);
        if (ret != 0)
        {
           cout << "pthread dataRecv() create error: error_code=" << ret << endl;
           exit(0);
        }


        ret = pthread_join(txt_tid, NULL);
        if (ret != 0)
        {
           cout << "pthread_join(txt_tid, NULL) error: error_code=" << ret << endl;
           exit(0);
        }
		ret = pthread_join(cmd_tid, NULL);
        if (ret != 0)
        {
           cout << "pthread_join(cmd_tid, NULL) error: error_code=" << ret << endl;
           exit(0);
        }
		ret = pthread_join(data_tid, NULL);
        if (ret != 0)
        {
           cout << "pthread_join(data_tid, NULL) error: error_code=" << ret << endl;
           exit(0);
        }

		printf("Vxworks shutdown.\n");
}








