#include <vector> 
#include<stdlib.h> 
#include <string>  
#include <fstream>  
#include <iostream>  
using namespace std;  

int main(int argc, char** argv)  
{  
	ifstream f1("check_cmd_from_vxworks.txt",ios::in);
	ifstream f2;
	if(argc==2 && atoi(argv[1])==1)
	{  
		f2.open("my_log.txt");
		cout<<"    To discover the difference between :"<<endl;
		cout<<"        check_cmd_from_vxworks.txt AND my_log.txt"<<endl;
		cout<<"    my_log.txt BY newLVCfg_GEMROC_TIGER.py"<<endl;
	}
	else
	{	
		f2.open("check_cmd_from_localhost.txt");
		cout<<"    To discover the difference between :"<<endl;
        cout<<"        check_cmd_from_vxworks.txt AND check_cmd_from_localhost.txt"<<endl;

	}
	ofstream f3("jie_guo_for_cmd_check.txt",ios::out);  
	string s1,s2,cmd1,cmd2;
	int i,j,num,len;  
	if (!f1.is_open()||!f2.is_open()||!f3.is_open())  
	{  
  		cout <<"open file fail"<<endl;  
    }  
	getline(f1,s1);
	getline(f2,s2);
	while(!f1.eof()||!f2.eof())
	{
		s1="";
		s2="";
		getline(f1,s1);
		getline(f2,s2);
		cmd1="";
		cmd2="";
		getline(f1,cmd1);
		getline(f2,cmd2);

		if(cmd1!=cmd2)
		{
			s1=s1+" from vxworks";
			s2=s2+" from localhost";
			f3<<s1<<endl;
			f3<<cmd1<<endl;
			f3<<s2<<endl;
			f3<<cmd2<<endl;
		}
	}
    f1.close();  
    f2.close(); 
    f3.close(); 

    return 0;  

}  
