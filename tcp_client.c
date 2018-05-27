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


int char_type(char c)
{
  if(c==' ')
    return 1;
  else if((c>='A'&&c<='Z')||(c>='a'&&c<='z'))
    return 2;
  else if(c>='0'&&c<='9')
    return 3;
  else
    return 0;
}

void extract_para_from_line(char*line,int line_strlen,char*cmd_string,unsigned int*cmd_para)
{
  int i,j,k,para_num, pre_char_type,para_info[10][3];
  unsigned int n;
  for(i=0;i<10;i++)
    for(j=0;j<3;j++)
      para_info[i][j]=0;
  pre_char_type=1; para_num=0;
  for(i=0;i<line_strlen-1;i++)
{
    if(pre_char_type==1&&char_type(line[i])==2)
   {  para_num=1;
      para_info[0][0]=1;
      para_info[0][1]=i;
      para_info[0][2]=i;}
    if(pre_char_type==2&&char_type(line[i])==2&&para_num==1)
      para_info[0][2]=i;
    if(pre_char_type==1&&char_type(line[i])==3&&para_num)
   {  para_num++;
      if(line[i]=='0'&&line[i+1]=='x'||line[i]=='0'&&line[i+1]=='X')
     {  para_info[para_num-1][0]=3;
        para_info[para_num-1][1]=i+2;
        para_info[para_num-1][2]=i+2; }
      else
     {  para_info[para_num-1][0]=2;
        para_info[para_num-1][1]=i;
        para_info[para_num-1][2]=i;}
    }
    if (para_num>1&&(line[i-1]>='0'&&line[i-1]<='9'||line[i-1]>='a'&&line[i-1]<='f'||line[i-1]>='A'&&line[i-1]<='F')&&(line[i]>='0'&&line[i]<='9'||line[i]>='a'&&line[i]<='f'||line[i]>='A'&&line[i]<='F'))
      para_info[para_num-1][2]=i;
    pre_char_type=char_type(line[i]);
 }
if(para_info[0][0]==1)
 {  j=0;
    for(i=para_info[0][1];i<=para_info[0][2];i++)
   {  cmd_string[j]=line[i];
      j++;}
    cmd_string[j]=0;}
  cmd_para[0]=para_num;
  for(i=1;i<cmd_para[0];i++)
{
    cmd_para[i]=0;
    if(para_info[i][0]==2)
      for(k=para_info[i][2];k>=para_info[i][1];k--)
     {
        n=1;
        for(j=para_info[i][2]-k;j>0;j--)
          n=n*10;
        cmd_para[i]=cmd_para[i]+(line[k]-48)*n;
     }
     if(para_info[i][0]==3)
        for(k=para_info[i][2];k>=para_info[i][1];k--)
       {
          n=1;
          for(j=para_info[i][2]-k;j>0;j--)
            n=n*16;
          if(line[k]>='0'&&line[k]<='9')
            cmd_para[i]=cmd_para[i]+(line[k]-48)*n;
          else if(line[k]>='A'&&line[k]<='F')
            cmd_para[i]=cmd_para[i]+(line[k]-55)*n;
          else
            cmd_para[i]=cmd_para[i]+(line[k]-87)*n;
        }  }  }




int main(int argc, char** argv)
{  
    char line[128],cmd_string[32];
    unsigned int cmd_para[10];
    int    sockfd;
    int i,len;
    struct sockaddr_in    servaddr;
    if( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
      printf("create socket error: %s(errno: %d)\n", strerror(errno),errno);
      exit(1);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(8000);
    inet_pton(AF_INET,"127.0.0.1", &servaddr.sin_addr);
    
    if( connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0)
    {
      printf("connect error: %s(errno: %d)\n",strerror(errno),errno);
      exit(1);
    }
    memset(cmd_string,0, sizeof(cmd_string));
    while(1)
    {
      if(strcmp(cmd_string,"quit")==0 ||strcmp(cmd_string ,"q")==0||strcmp(cmd_string,"Quit")==0||strcmp(cmd_string,"Q")==0)
      {
        line[0]='q';
        line[1]='Q';
        send(sockfd,line, 2,0);
        break;
      }
      else
      {
        memset(line,0, sizeof(line));
        line[0]='n';
        line[1]='N';
        send(sockfd,line, 2,0);
        memset(cmd_string,0, sizeof(cmd_string));
        memset(line,0, sizeof(line));
        len = recv(sockfd, line, 128,0) ;
        extract_para_from_line(line,strlen(line),cmd_string,cmd_para);
        if(cmd_para[0]>0)
        {
           printf("%s ",cmd_string);
           for(i=1;i<cmd_para[0];i++)
             printf("%d ",cmd_para[i]);
           printf("\n");
             
        }
      }
    }

close(sockfd);
}
