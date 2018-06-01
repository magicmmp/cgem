
#include "stdio.h"
#include "ioctl.h"		//for ioctl
#include "ioLib.h"		//for ioctl

#include "fcntl.h"
#include "logLib.h"

#include "fw_dcgvas.h"
#include "fw_tcpfunc.h"
#include "fw_staterrspt.h"

extern  struct inpcbhead tcpcb;  //parameter of OS

int 	CmdStatSockConnect()	//setup local server, from address initializaion to listen
{
	int nAdrsLen = sizeof(struct sockaddr_in) ;
	struct sockaddr_in SockLocalAdrs;
	struct sockaddr_in SockRemoteAdrs;

	g_sktListen = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP) ;
	if(ERROR == g_sktListen)
	{
		printf("socket failed\n") ;
		return -1;
	}
	bzero((char*)&SockLocalAdrs, nAdrsLen) ;
	SockLocalAdrs.sin_family = AF_INET ;
	SockLocalAdrs.sin_port =htons(LOCAL_SERV_PORT) ;
	SockLocalAdrs.sin_addr.s_addr = INADDR_ANY ;
	
	int ReUseSockOpt=1;	//for shutdown not reboot
	if(ERROR==setsockopt(g_sktListen,SOL_SOCKET,SO_REUSEADDR,(char*)&ReUseSockOpt,sizeof(ReUseSockOpt)))
	{
		printf("set sock options error %d\n",ERROR);
	}
	
	
	if(ERROR == bind(g_sktListen, (struct sockaddr*)&SockLocalAdrs, nAdrsLen))
	{
		printf("bind failed\n") ;
		return -1;
	}

	if(ERROR == listen(g_sktListen, 10))
	{
		printf("listen failed\n") ;
		return -1;
	}
		
	/*-----------accept connect-------------------*/
	bzero((char*)&SockRemoteAdrs, nAdrsLen) ;
	g_sktCmdAck = accept(g_sktListen, (struct sockaddr*)&SockRemoteAdrs, &nAdrsLen) ;
	if (-1 == g_sktCmdAck)
	{
		printf("accept failed\n") ;
		return -1;
	}
	
	char cIPStr[32];
	inet_ntoa_b (SockRemoteAdrs.sin_addr, cIPStr);//convert to IP string
	DEBUG_OUTPUT( ("g_sktCmdAckCli connected\n")) ;
	DEBUG_OUTPUT( ("g_sktCmdAck, Remote Address: %s, Port: %d\n", 
						cIPStr, ntohs(SockRemoteAdrs.sin_port)) );
	
	bzero((char*)&SockRemoteAdrs, nAdrsLen) ;
	g_sktStaInfo = accept(g_sktListen, (struct sockaddr*)&SockRemoteAdrs, &nAdrsLen) ;
	if (-1 == g_sktStaInfo)
	{
		printf("accept failed\n") ;
		return -1;
	}
	
	inet_ntoa_b(SockRemoteAdrs.sin_addr, cIPStr);//convert to IP string
	
	DEBUG_OUTPUT(("g_sktStaInfo connected\n")) ;
	DEBUG_OUTPUT(("g_sktStaInfo, Remote Address: %s, Port: %d\n", 
						cIPStr, ntohs(SockRemoteAdrs.sin_port)) );
	
	return 0 ;
}

int DataSockConnect(int nDataIP,unsigned short nDataPort)  //return state,connect non block sock
{
	struct sockaddr_in SockRemoteAdrs;
	SockRemoteAdrs.sin_family =AF_INET;
	SockRemoteAdrs.sin_port = htons(nDataPort);  /*type convert */
	SockRemoteAdrs.sin_addr.s_addr = htonl(nDataIP) ;
	if( (g_sktData= socket(AF_INET, SOCK_STREAM, IPPROTO_TCP))==ERROR)
	{
		DEBUG_OUTPUT(("g_sktData init fail\n"));
		Push2ErrStack(PPCERROR+FRM_FLG+ATOM_ERR+53,0,0,0,0);			/*error 53*/
		return -1;
	}

	if (connect(g_sktData, (struct sockaddr *)&SockRemoteAdrs,sizeof(struct sockaddr)) == -1)
	{
		DEBUG_OUTPUT(("g_sktData connect server fail\n"));
		Push2ErrStack(PPCERROR+FRM_FLG+ATOM_ERR+54,0,0,0,0);			/*error 54*/
		return -1;
	}
	int bIOcontrol=1;
	if(-1== ioctl (g_sktData, FIONBIO, bIOcontrol))		//non block
	{
		DEBUG_OUTPUT(("set NonBlock data Sokcet I/O error\n"));
		Push2ErrStack(PPCERROR+FRM_FLG+ATOM_ERR+55,0,0,0,0);			/*error 55*/
		return -1;
	}
	if( (g_DataTcpcb=GetSpecTcpcb(g_sktData,nDataPort) )==NULL)
	{
		DEBUG_OUTPUT(("get g_sktData Tcpcb fail\n"));
		Push2ErrStack(PPCERROR+FRM_FLG+ATOM_ERR+56,0,0,0,0);			/*error 56*/
		return -1;
	}
	return 0;
}
	
struct tcpcb* GetSpecTcpcb(int nfd,unsigned short port)
{
	struct inpcb   *pfst;
	struct tcpcb   *pTcPcb;
	pfst = tcpcb.lh_first;
	while (pfst)
	{
		if( ((pfst->inp_fport)==port)&&( (pfst->inp_socket->so_fd)==nfd ))
		{
			pTcPcb=(struct tcpcb*)(pfst->inp_ppcb);
			printf("nfd tcp pcb point ok\n");
			return pTcPcb;
		}
	 	pfst = pfst->inp_list.le_next;
	}
	return NULL;
}

int SelectSend(int sockfd,char *psndbuf,int nsndlen,struct tcpcb * pTcpPcb)
{
	int	nSndLenTmp;
	int	nHaveSend;
	char * psndbufTmp;
	int	nSelTimer=0;
	
	fd_set 	writefds;
	struct timeval tv;

	//return nsndlen;

	nSndLenTmp=nsndlen;
	psndbufTmp=psndbuf;
	while(nSndLenTmp)
	{
		FD_ZERO(&writefds);
		FD_SET(sockfd,&writefds);
		if(nSelTimer==0)
		{
			tv.tv_sec = 0;
			tv.tv_usec = 500000;
		}
		else
		{
			tv.tv_sec = 1;
			tv.tv_usec = 0;
		}
		switch (select(sockfd+ 1, NULL, &writefds,NULL, &tv)) 
		{  
			case -1:  
				logMsg("select error\n", 0, 0, 0, 0, 0, 0);
				break;  
			case 0:  
				//printf("pTcpPcb->t_timer[0] = %d\n", pTcpPcb->t_timer[0]);
				if(pTcpPcb->t_timer[0] > 1) 
				{
					printf("pTcpPcb->t_timer[0] = %d\n", pTcpPcb->t_timer[0]);
					pTcpPcb->t_timer[0]=1;
				}
				if(nSelTimer==0)
				{
					logMsg("sockfd time out 0.5 second\n", 0, 0, 0, 0, 0, 0);
				}
				else
				{
					logMsg("sockfd time out 1 second\n", 0, 0, 0, 0, 0, 0);
				}
				nSelTimer++;
				taskDelay(60);
				break;
			default:
			{	if (FD_ISSET(sockfd,&writefds))
				{
					nSelTimer=0;
					nHaveSend=send(sockfd,psndbufTmp,nSndLenTmp,MSG_DONTWAIT);
					if(nHaveSend<0)
					{
						if ((errno ==EAGAIN)||(errno ==EWOULDBLOCK)||(errno == EINTR)) 
						{
							nHaveSend = 0;          
						}
						else
						{
							//logMsg("select socket send error,%d\n", errno, 0, 0, 0, 0, 0);
							perror("select socket send error ");
							Push2ErrStack(PPCFATAL+FRM_FLG+ATOM_ERR+57,0,0,0,0);			/*error 57*/
							return -1;
						}
					}
					psndbufTmp+=nHaveSend;
					nSndLenTmp-=nHaveSend;
				}
				break;
			}
		}
	}
	return nsndlen;
}

int Snd(int sockfd,char *psndbuf,int nsndlen)
{
	int	nSndLenTmp;
	int	nHaveSend;
	char * psndbufTmp;

	//return nsndlen;

	nSndLenTmp=nsndlen;
	psndbufTmp=psndbuf;

	while(nSndLenTmp)
	{	
		nHaveSend=send(sockfd,psndbufTmp,nSndLenTmp,MSG_DONTWAIT);
		if(nHaveSend<0)
		{
			if ((errno ==EAGAIN)||(errno ==EWOULDBLOCK)||(errno == EINTR)) 
			{
				nHaveSend = 0;          
			}
			else
			{
				//logMsg("select socket send error,%d\n", errno, 0, 0, 0, 0, 0);
				perror("socket send error ");
				Push2ErrStack(PPCFATAL+FRM_FLG+ATOM_ERR+57,0,0,0,0);			/*error 57*/
				return -1;
			}
		}
		psndbufTmp+=nHaveSend;
		nSndLenTmp-=nHaveSend;
	}
	return nsndlen;
}
int Recv(int fd, char * vptr, int nbytes)
{
	int   		nleft;
	int  		nRecv;
	char *	ptr;
	
	ptr = (char *)vptr;
	nleft = nbytes ;

	while (nleft > 0) 
	{
		if ( (nRecv = recv(fd, ptr, nleft, MSG_WAITALL)) <0) //MSG_WAITALL may don't return 0 bytes
		{
			printf("nRecv=%d\n",nRecv);
			perror("sorry:");
			if ((errno ==EAGAIN)||(errno ==EWOULDBLOCK)||(errno == EINTR)) 
			{
				nRecv = 0;          
			}
			else
			{
				return -1 ;	/* error*/
			}
		}
		
		nleft -= nRecv;
		ptr   += nRecv;
	}
	return(nbytes);
	
}

int CloseDataSocket()
{
	if (g_sktData)
	{
		if(ERROR==close(g_sktData) )
		{
			DEBUG_OUTPUT(("Close g_sktData Failed! \n"));
			return -1;
		}
		g_sktData=0;
		g_DataTcpcb=NULL;
	}
	DEBUG_OUTPUT(("CloseDataSocket SUCC\n"));
	return 0;
}

int CloseCmdStatSocket()
{
	if (g_sktCmdAck)
	{
		if(ERROR==close(g_sktCmdAck) )
		{
			DEBUG_OUTPUT(("Close g_sktCmdAck Failed! \n"));
			return -1;
		}
		g_sktCmdAck=0;
	}
	if (g_sktStaInfo)
	{
		if(ERROR==close(g_sktStaInfo) )
		{
			DEBUG_OUTPUT(("Close g_sktStaInfo Failed! \n"));
			return -1;
		}
		g_sktStaInfo=0;
	}
	if (g_sktListen)
	{
		if(ERROR==close(g_sktListen) )
		{
			DEBUG_OUTPUT(("Close g_sktListen Failed! \n"));
			return -1;
		}
		g_sktListen=0;
	}
	DEBUG_OUTPUT(("CloseCmdStatSocket SUCC\n"));
	return 0;
}

int CloseAllSockets()
{
	if(-1==CloseDataSocket())
	{
		DEBUG_OUTPUT(("CloseDataSocket Failed! \n"));
		return -1;
	}
	if(-1==CloseCmdStatSocket() )
	{
		DEBUG_OUTPUT(("CloseCmdStatSocket Failed! \n"));
		return -1;
	}
	DEBUG_OUTPUT(("CloseAllSockets SUCC! \n"));
	return 0;
}

