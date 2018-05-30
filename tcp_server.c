#include<stdio.h>
#include<stdlib.h>
#include<netinet/in.h>
#include <sys/types.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include <unistd.h>
#include<netdb.h>
#include<string.h>
#include <sys/time.h>
#include <netinet/tcp.h>
#include <errno.h>
#define DEFAULT_PORT 8000

int main(int argc, char** argv)
{
    char line[128];
    FILE *fd_read_para=NULL;
    int len;
    int    socket_fd, connect_fd;
    struct sockaddr_in   cliaddr,  servaddr;
    socklen_t cli_len = sizeof(cliaddr);
    fd_read_para=fopen("auto_input_para.txt","r");
    if( (socket_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1 )
    {
      printf("create socket error: %s(errno: %d)\n",strerror(errno),errno);
      exit(0);
    }
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    printf("input = 1,vxworks;  else itself\n");
    if(argc==2 && atoi(argv[1])==1)
      servaddr.sin_addr.s_addr = inet_addr("192.168.1.200");
    else
      servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    servaddr.sin_port = htons(DEFAULT_PORT);

    if( bind(socket_fd, (struct sockaddr*)&servaddr, sizeof(servaddr)) == -1)
    {
      printf("bind socket error: %s(errno: %d)\n",strerror(errno),errno);
      exit(0);
    }
    if( listen(socket_fd, 10) == -1)
    {
      printf("listen socket error: %s(errno: %d)\n",strerror(errno),errno);
      exit(0);
    }
    printf("\n======waiting for client's request======\n");
    if( (connect_fd = accept(socket_fd, (struct sockaddr*)&cliaddr, &cli_len)) == -1)
    {
      printf("accept socket error: %s(errno: %d)",strerror(errno),errno);
    }
    else
      printf("accept connect: %s,port:%d\n",inet_ntoa(cliaddr.sin_addr),cliaddr.sin_port);
    while(1)
    {
      memset(line,0, sizeof(line));
      len = recv(connect_fd,line, 128,0) ;
      if(line[0]=='n' && line[1]=='N')
      {
        printf("recv cmd = %s\n",line);
        memset(line,0, sizeof(line));
        fgets(line, sizeof(line), fd_read_para);
        send(connect_fd,line, 128,0);
      }
      else
      {
        printf("\nrecv cmd = %s\n",line);
        printf("exit.\n");
        break;
      }
    }
    fclose(fd_read_para);
    close(connect_fd);
    close(socket_fd);
}
