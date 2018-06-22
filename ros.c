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
/** gcc -D_REENTRANT ros.c -o ros -lpthread   */
struct Header
       {
	unsigned int	All_len;
	unsigned int	crate_id;
	unsigned int	crate_len;
	unsigned int	trigger_type;
	unsigned int	module_num;
	unsigned int	events_per_read;
	unsigned int	run_times;
	unsigned int	unused;	
       };
struct GE_ppc		
       {
	unsigned int	info_len;
	unsigned int	module_type;
	unsigned int	geo;
	unsigned int	PPC_id;
	unsigned int	ROS_ip;
	unsigned int	ROS_port;
	unsigned int	NormModeSelfL1;	
      };
struct Header h;
struct GE_ppc ppc;
int    fd_cmdack,fd_stainfo,fd_data;
struct sockaddr_in    servaddr,local_addr,vxaddr;
const char *g_strCmd[15]={"LOAD","PRCF","CONF","SYN1","SYN2",
                          "SYN3","SYN4","PREP","SATR","SPTR",
                          "STOP","UNCF","UNPC","UNLD","EXIT"};

void *cmdack()
{
    int i,j;
    char echo[16];
    char config[256];
    char config0[256];
    h.All_len=sizeof(h)+sizeof(ppc);
    h.crate_id=240;
    h.crate_len=sizeof(h);
    ppc.info_len=sizeof(ppc);
    ppc.module_type=0xf1;
    ppc.PPC_id=240;
    ppc.ROS_ip=htonl(local_addr.sin_addr.s_addr);
    ppc.ROS_port=8800;
    memset(config0, 0, sizeof(config0));
    memcpy(config0, &h, sizeof(h)) ;
    memcpy(config0+sizeof(h), &ppc, sizeof(ppc)) ;
    for(i=0;i<256;i=i+4)
      for(j=0;j<4;j++)
        config[i+j]=config0[i+3-j];
    send(fd_cmdack,g_strCmd[0],4,0);
    send(fd_cmdack,config,4,0);
    send(fd_cmdack,config+4,h.All_len-4,0);
    recv(fd_cmdack,echo,16,0);
    printf("recv cmd echo : %s.\n",echo);
    for(i = 1;i < 9;i++)
    {
       send(fd_cmdack,g_strCmd[i],4,0);
       printf("send cmd : %s.\n",g_strCmd[i]);
       recv(fd_cmdack,echo,16,0);
       printf("recv cmd echo : %s.\n",echo);
       sleep(1);
    }

}
void *data_recv()
{
    int i;
    char buff[3200];
    int connect_fd;
    int len,sin_len;
    if( listen(fd_data, 10) == -1)
    {
      printf("listen socket fd_data error.\n");
    }
    if( (connect_fd = accept(fd_data, (struct sockaddr *)&vxaddr,&sin_len)) == -1)
    {
      printf("accept socket error.");
    }
    printf("accept connection from vxworks,IP=%s,port=%d.\n",inet_ntoa(vxaddr.sin_addr),vxaddr.sin_port);
    len=recv(fd_data,buff,3200,0);
    printf("recv from fd_data : %s, len=%d.\n",buff,len);
    len=-1;
    while(len==-1)
      len=recv(fd_data,buff,3200,0);
    while(1)
    {   
       len=recv(fd_data,buff,3200,0);
       printf("recv data %d bytes.\n",len);
    }
}
void *stainfo()
{
    int i;
    int len;
    char info[2048];
    while(1)
    {
        len = recv(fd_stainfo,info,2048,0) ;
        printf("fd_stainfo recv %d bytes.\n",len);
    }

}
int main()
{  

    fd_cmdack = socket(AF_INET, SOCK_STREAM, 0);
    fd_stainfo = socket(AF_INET, SOCK_STREAM, 0);
    fd_data = socket(AF_INET, SOCK_STREAM, 0);

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(9240);
    inet_pton(AF_INET,"192.168.1.201", &servaddr.sin_addr);
    
    memset(&local_addr, 0, sizeof(local_addr));
    local_addr.sin_family = AF_INET;
    local_addr.sin_port = htons(8800);
    inet_pton(AF_INET,"192.168.1.200", &local_addr.sin_addr);
    if( bind(fd_data, (struct sockaddr*)&local_addr, sizeof(local_addr)) == -1)
    {
      printf("bind fd_data error.\n");
    }

    connect(fd_cmdack, (struct sockaddr*)&servaddr, sizeof(servaddr));
    connect(fd_stainfo, (struct sockaddr*)&servaddr, sizeof(servaddr));
    
    pthread_t id1;
    pthread_t id2;
    pthread_t id3;
    int ret ;
    ret = pthread_create(&id1, NULL, cmdack, NULL);
    if(ret)
    {
        printf("Create pthread cmdack error!\n");
        return 1;
    }
    ret = pthread_create(&id2, NULL, stainfo, NULL);
    if(ret)
    {
        printf("Create pthread stainfo error!\n");
        return 1;
    }
    ret = pthread_create(&id3, NULL, data_recv, NULL);
    if(ret)
    {
        printf("Create pthread data_recv error!\n");
        return 1;
    }
    printf("before.\n");    
    pthread_join(id1, NULL);
    printf("end thread 1.\n");
    pthread_join(id2, NULL);
    printf("end thread 2.\n");
    pthread_join(id3, NULL);
    printf("end thread 3.\n");
close(fd_cmdack);
close(fd_stainfo);
close(fd_data);
}
