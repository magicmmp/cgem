#include <stdio.h>

int main()
{
   char *p;
   unsigned int x=0x12345678;
   printf("x=0x%08X\n",x);
   int i;
   p=(char*)&x;
   printf("char:");
   for(i=0;i<4;i++)
     printf("%02X ",p[i]);
   printf("\n");

}
