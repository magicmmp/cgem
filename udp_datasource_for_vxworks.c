#include <stdio.h>  
#include <stdlib.h>  
#include <string.h>  
#include <sys/socket.h>  
#include <netinet/in.h>  
#include <arpa/inet.h>  
#include <netdb.h>   

  
int main(int argc, char** argv) 
{  
    int socket_descriptor;
    char buf[512];
    int i;
    unsigned char n;
    struct sockaddr_in address;
    bzero(&address,sizeof(address));  
    address.sin_family=AF_INET; 

    printf("Input para =0 ,send to 127.0.0.1,else to vxWorks.\n");
    if(argc==2 && atoi(argv[1])==0)
	address.sin_addr.s_addr=inet_addr("127.0.0.1");
    else
    	address.sin_addr.s_addr=inet_addr("192.168.1.201");
    address.sin_port=htons(58914);  
    socket_descriptor=socket(AF_INET,SOCK_DGRAM,0);
    n=0;
    while(1)  
    {
      for(i=0;i<512;i++)
        buf[i]=n;
      n++;
      sendto(socket_descriptor,buf,sizeof(buf),0,(struct sockaddr *)&address,sizeof(address));
      
      sleep(5); 
    }   
    close(socket_descriptor);  
    printf("Messages Sent\n");    
}  

