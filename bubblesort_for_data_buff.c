#include<stdio.h>
void swap_8_bytes(char*p)
{
    int i;
    char tmp[8];
    for(i=0;i<8;i++)
      tmp[i]=p[i];
    for(i=0;i<8;i++)
      p[i]=p[i+8];
    for(i=0;i<8;i++)
      p[i+8]=tmp[i];

}
void sort(char*s,int p1;int p2)
{
    int i,j;
    char tmp[8];
    for(i=0;i<p2-p1;i=i+8)
      for(j=p1;j<p2-i;j=j+8)
      {
        if(s[j]>s[j+8])
      }


}



int main()
{  
   char i;
   char s[32];
   for(i=0;i<32;i++)
     s[i]=32-i;
   for(i=0;i<32;i++)
     printf("%d ",s[i]);
   printf("\n");
   swap_8_bytes(s);
   for(i=0;i<32;i++)
     printf("%d ",s[i]);
   printf("\n");


}
