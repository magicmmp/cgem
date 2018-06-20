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



void *cmdack()
{
    int i;
    for(i = 0;i < 3;i++)
    {

        printf("This is a cmdack thread.\n");
    }

}
void *stainfo()
{
    int i;
    for(i = 0;i < 3;i++)
    {

        printf("This is a stainfo thread.\n");
    }

}
int main()
{  
    int    fd_cmdack,fd_stainfo;
    
    struct sockaddr_in    servaddr;

    fd_cmdack = socket(AF_INET, SOCK_STREAM, 0);
    fd_stainfo = socket(AF_INET, SOCK_STREAM, 0);

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(9230);
    inet_pton(AF_INET,"192.168.1.201", &servaddr.sin_addr);
    /**
    connect(fd_cmdack, (struct sockaddr*)&servaddr, sizeof(servaddr));
    connect(fd_stainfo, (struct sockaddr*)&servaddr, sizeof(servaddr));
    */
    pthread_t id1;
    pthread_t id2;
    int ret = pthread_create(&id1, NULL, cmdack, NULL);
    if(ret)
    {
        printf("Create pthread error!\n");
        return 1;
    }
    ret = pthread_create(&id2, NULL, stainfo, NULL);
    if(ret)
    {
        printf("Create pthread error!\n");
        return 1;
    }
    printf("before.\n");    
    pthread_join(id1, NULL);
    printf("end thread 1.\n");
    pthread_join(id2, NULL);
    printf("end thread 2.\n");

close(fd_cmdack);
close(fd_stainfo);
}
