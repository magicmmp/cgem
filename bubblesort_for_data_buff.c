#include<stdio.h>

void sort(char*s,int p1,int p2)
{
    int i,j,k;
    char tmp[8];
    for(i=0;i<p2-p1;i=i+8)
      for(j=p1;j<p2-i;j=j+8)
      {
        if(s[j]>s[j+8])
        {
          for(k=0;k<8;k++)
            tmp[k]=s[j+k];
          for(k=0;k<8;k++)
            s[j+k]=s[j+k+8];
          for(k=0;k<8;k++)
            s[j+k+8]=tmp[k];
        }
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
   sort(s,0,24);
   for(i=0;i<32;i++)
     printf("%d ",s[i]);
   printf("\n");


}