#include <stdio.h>
#include <string.h>

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
{int i,j,k,para_num, pre_char_type,para_info[10][3];
 unsigned int n;
for(i=0;i<10;i++)
for(j=0;j<3;j++)
para_info[i][j]=0;
pre_char_type=1; para_num=0;
for(i=0;i<line_strlen-1;i++)
{ if(pre_char_type==1&&char_type(line[i])==2)
{ para_num=1;para_info[0][0]=1;para_info[0][1]=i;para_info[0][2]=i;}
if(pre_char_type==2&&char_type(line[i])==2&&para_num==1)  para_info[0][2]=i;
if(pre_char_type==1&&char_type(line[i])==3&&para_num)
{  para_num++;
if(line[i]=='0'&&line[i+1]=='x'||line[i]=='0'&&line[i+1]=='X')
{para_info[para_num-1][0]=3;para_info[para_num-1][1]=i+2;para_info[para_num-1][2]=i+2; }  
else
{para_info[para_num-1][0]=2;para_info[para_num-1][1]=i;para_info[para_num-1][2]=i;}
}
if(para_num>1&&(line[i-1]>='0'&&line[i-1]<='9'||line[i-1]>='a'&&line[i-1]<='f'||line[i-1]>='A'&&line[i-1]<='F')&&(line[i]>='0'&&line[i]<='9'||line[i]>='a'&&line[i]<='f'||line[i]>='A'&&line[i]<='F')) 
para_info[para_num-1][2]=i;
pre_char_type=char_type(line[i]);   }
if(para_info[0][0]==1)
{j=0;
for(i=para_info[0][1];i<=para_info[0][2];i++)
{cmd_string[j]=line[i];j++;}
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
cmd_para[i]=cmd_para[i]+(line[k]-87)*n;}  }  }


int main()
{
   FILE *fp = NULL;
   char line[128],cmd_string[32];
   unsigned int cmd_para[10];
   int i,j;
   fp = fopen("auto_input_para.txt", "r");
 while(!feof(fp))  
  {  
       memset(cmd_string,0, sizeof(cmd_string));
       memset(line,0, sizeof(line));  
       fgets(line, sizeof(line), fp);
       extract_para_from_line(line,strlen(line),cmd_string,cmd_para);
       printf("string length=%d,para_num=%d,cmd string:%s,",strlen(cmd_string),cmd_para[0],cmd_string);
       for(i=1;i<cmd_para[0];i++)
       printf(" %u",cmd_para[i]);
       printf("\n");
    }  



   fclose(fp);
return 0;
}



