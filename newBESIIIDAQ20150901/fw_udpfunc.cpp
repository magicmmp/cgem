
#include "vxWorks.h"
#include "stdio.h"
#include "taskLib.h"
#include "sysLib.h"

#include "fw_define.h"
#include "fw_dcgvas.h"
#include "fw_udpfunc.h"
#include "fw_staterrspt.h"


/*below is the status show code
STID_Waiting 			0	sendcmd 1
STID_Initialized 		1	sendcmd 2
STID_Loaded 			2	sendcmd 3
STID_PreConfiged 		3	sendcmd 4
STID_Configed 		4	sendcmd 5
STID_Synchronized1	5	sendcmd 6
STID_Synchronized2 	6	sendcmd 7
STID_Synchronized3 	7	sendcmd 8
STID_Synchronized4 	8	sendcmd 9
STID_Ready 			9	sendcmd 10
STID_Running 		10	sendcmd 11
rebooting..			11	sendcmd 12
*/
//command
#define UDPREQUEST 14
#define UDPREBOOT 15
#define UDPUPDATEFPGA 16
//state
#define REBOOTING 12


int 	udprecvfd;
struct sockaddr_in   loca_serv_addr;
struct sockaddr_in   remo_addr;

int StartUdpMonitor()
{
	int addrlen=sizeof(struct sockaddr_in);
	int bReuseSock=1;
	udprecvfd=socket(AF_INET,SOCK_DGRAM,0);
	setsockopt(udprecvfd,SOL_SOCKET,SO_REUSEADDR,(char*)&bReuseSock,sizeof(bReuseSock));
	if(udprecvfd<0)
	{
		printf("StartUdpMonitor Socket  Error:%s\n",strerror(errno));
       	close(udprecvfd);
		Push2ErrStack(PPCWARNING+FRM_FLG+ATOM_ERR+58,0,0,0,0);		/*error 58*/
		return -1;
	}
	//----------------recv-------------
	bzero((char*)&loca_serv_addr,sizeof(struct sockaddr_in));
	loca_serv_addr.sin_family=AF_INET;
	loca_serv_addr.sin_addr.s_addr=htonl(INADDR_ANY);
	loca_serv_addr.sin_port=htons(44444);
	if(bind(udprecvfd,(struct sockaddr *)&loca_serv_addr,sizeof(struct sockaddr_in))<0)
	{
		printf("StartUdpMonitor Bind Error:%s\n",strerror(errno));
		Push2ErrStack(PPCWARNING+FRM_FLG+ATOM_ERR+59,0,0,0,0);		/*error 59*/
		return -1;
	}
	//----------------send-------------
	//bzero((char*)&remo_serv_addr,sizeof(struct sockaddr_in));
	//remo_serv_addr.sin_family=AF_INET;
	//remo_serv_addr.sin_port=htons(2222);
	//if(inet_aton(cUdpCnectIp,&remo_serv_addr.sin_addr)<0)
	//{
	//	printf("Ip error:%s\n",strerror(errno));
	//	return -1;
	//}
	if(-1 == taskSpawn("tUdpwatchstat", PRI_WAITING, 0, TASK_STACK_SIZE, (FUNCPTR)UdpWatchStat, 
		0, 0, 0, 0, 0, 0, 0, 0, 0,0))
	{
		printf("StartUdpMonitor taskSpawn tWaiting failed\n") ;
		Push2ErrStack(PPCWARNING+FRM_FLG+ATOM_ERR+60,0,0,0,0);		/*error 60*/
		return -1;
	}
	return 0;
}


void UdpWatchStat()
{
	char msg[16]={16*'\0'};
	int 	nUdpCmd=0;
	
	int addrlen=sizeof(struct sockaddr_in);
	
	bzero((char*)&remo_addr,sizeof(struct sockaddr_in));
	while(1)
	{
		int nrecv=recvfrom(udprecvfd,msg,4,0,(struct sockaddr*)&remo_addr,&addrlen);
//		printf("recv-%d from %s server,port %d\n",*((int*)msg),inet_ntoa(remo_addr.sin_addr),remo_addr.sin_port);
		nUdpCmd=*((int*)msg);
		
		if(nUdpCmd==UDPREBOOT)
		{
			//UdpSendCurStat(REBOOTING);
			printf("recv reboot command\n") ;
			break;
		}
		if(nUdpCmd==UDPREQUEST)
		{
			UdpSendCurStat(g_nCurnState+1);
		}
	}
	ioctl (udprecvfd, FIORFLUSH, 0);//clear all the read buffer
       close(udprecvfd);

	taskDelay(rand()%300);
	reboot(2);
}
void UdpSendCurStat(int nStat)
{
	char ppcstate[8];
	*((int*)ppcstate)=nStat;
	int n=sendto(udprecvfd,(caddr_t)ppcstate,4,0,(struct sockaddr*)&remo_addr,sizeof(struct sockaddr_in));
	//printf("n=%d,nState=%d\n",n,nStat);
	//int n=sendto(udpsendfd,(caddr_t)ppccmd,strlen(ppccmd),0,(struct sockaddr*)&remo_serv_addr,sizeof(struct sockaddr_in));
	//int n=sendto(udpsendfd,(caddr_t)ppccmd,8,0,(struct sockaddr*)&remo_serv_addr,sizeof(struct sockaddr_in));
	//printf("send stat: %s ,%d,%s,prot %d\n",ppccmd,n,inet_ntoa(remo_serv_addr.sin_addr),remo_serv_addr.sin_port);
}
	