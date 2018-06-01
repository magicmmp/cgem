#ifndef __INCFWTCPFUNh
#define __INCFWTCPFUNh

int 	CmdStatSockConnect();
int 	DataSockConnect(int nDataIP,unsigned short nDataPort);//return sockfd

struct tcpcb* GetSpecTcpcb(int nfd,unsigned short port);
int 	SelectSend(int sockfd,char *psndbuf,int nsndlen,struct tcpcb * pTcpPcb);
int Snd(int sockfd,char *psndbuf,int nsndlen);
int 	Recv(int fd, char * vptr, int nbytes) ;

int CloseDataSocket();
int CloseCmdStatSocket();
int CloseAllSockets();
#endif

