#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
/** gcc -D_REENTRANT multi_thread_test.c -o multi_thread_test -lpthread   */
void *thread()
{
    int i;
    for(i = 0;i < 3;i++) 
    {
        
        printf("This is a pthread.\n");
    }
   
}

int main() 
{
    pthread_t id;
    int ret = pthread_create(&id, NULL, thread, NULL);
    if(ret) 
    {
        printf("Create pthread error!\n");
        return 1;
    }
    int i;
    for(i = 0;i < 3;i++) 
    {
        printf("This is the main process.\n");
        
    }
    pthread_join(id, NULL);
    return 0;
}

 
