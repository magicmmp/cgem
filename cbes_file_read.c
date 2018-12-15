#include <stdio.h>

int main()
{
	FILE* fp = fopen("run_0010013_All_file001_SFO-1.raw", "r");
	unsigned char buff[1024];
	fread(buff,sizeof(unsigned char),1024,fp);
	fclose(fp);
	int i,j;
	for(i=0;i<128;i=i+4)
    {
        unsigned int tmp=0;
        for(j=3;j>0;j--)
        {
            tmp=tmp|buff[i+j];
            tmp=tmp<<8;
        }
        tmp=tmp+buff[i];
        printf("%08X\n",tmp);
    }







}
