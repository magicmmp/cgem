#include <stdio.h>  
void readfromtxt(const char* txt_file_name,unsigned int* buf,unsigned int len)
{
   FILE *fp = NULL;
   int i;
   fp = fopen(txt_file_name, "r");
    for( i=0;i<len;i++)
      fscanf(fp,"%d",&buf[i]); 
   fclose(fp);
}
int main()
{
   FILE *fp = NULL;
   int  a[50];
   int i;
char gemroc_command_list[6][32] = {"NONE",
                            "CMD_GEMROC_LV_CFG_WR",
                            "CMD_GEMROC_LV_CFG_RD",
                            "CMD_GEMROC_LV_IVT_UPDATE",
                            "CMD_GEMROC_LV_IVT_READ",
                            "CMD_GEMROC_TIMING_DELAYS_UPDATE"};
for(i=0;i<6;i++)
printf("i=%d,len=%d %s\n",i,sizeof(gemroc_command_list[i]),gemroc_command_list[i]);
if(strcmp(gemroc_command_list[2],"CMD_GEMROC_LV_IVT_UPDATE")==0)
printf("ture\n");
else
printf("false\n");

//   fp = fopen("TIGER_def_g_cfg_2018.txt", "r");
//   fread(buff,sizeof(unsigned char),1024,fp);
for( i=0;i<37;i++)  
  printf("%d ",a[i]);          
printf("\n");         
                                         
readfromtxt("TIGER_def_g_cfg_2018.txt",a,37);

for( i=0;i<37;i++)
  printf("%d ",a[i]); 
printf("\n");

return 1;
}
