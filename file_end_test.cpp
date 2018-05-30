#include <vector>  
#include <string>  
#include <fstream>  
#include <iostream>  
using namespace std;  

int main()  
{  
    ifstream f1("auto_input_para.txt",ios::in);  
    string s1;
    if (!f1.is_open())  
    {  
      cout <<"open file fail"<<endl;  
    }  

    while(!f1.eof())
{
getline(f1,s1);
cout <<s1<<endl;

}
    f1.close();  

    return 0;  

}  
