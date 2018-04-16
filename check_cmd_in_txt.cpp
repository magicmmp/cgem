#include <vector>  
#include <string>  
#include <fstream>  
#include <iostream>  
using namespace std;  

int main()  
{  
ifstream f1("my_log.txt",ios::in);  
ifstream f2("my_c_log.txt",ios::in);
ofstream f3("jie_guo_for_cmd_check.txt",ios::out);  
string s1,s2,cmd1,cmd2;
int i,j,num,len;  
if (!f1.is_open()||!f2.is_open())  
{  
  cout <<"open file fail"<<endl;  
    }  
getline(f1,s1);
getline(f2,s2);
for(i=0;i<140;++i)
{
getline(f1,s1);
getline(f2,s2);
getline(f1,cmd1);
getline(f2,cmd2);

if(cmd1!=cmd2)
{
f3<<s1<<endl;
f3<<cmd1<<endl;
f3<<s2<<endl;
f3<<cmd2<<endl;
}
}
/*   while(getline(myfile,temp))  
    {  

        outfile << temp;  
        outfile << endl;
    }  
*/
    f1.close();  
    f2.close(); 
    f3.close(); 

    return 0;  

}  
