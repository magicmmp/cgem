#include <iostream>
#include<fstream>
#include<string>
#include<vector>
#include<cstdio>
using namespace std;

int main()
{
    ifstream infile("run_0010013_All_file001_SFO-1.raw",ios::in | ios::binary);
    if(!infile)
    {
        cerr<<"cannot open file."<<endl;
        return -1;
    }
    unsigned char buff[1024];
    infile.read((char*)buff,1024);
    
    for(int i=0;i<128;i=i+4)
    {
		unsigned int tmp=0;
		for(int j=3;j>0;j--)
        {
			tmp=tmp|buff[i+j];
			tmp=tmp<<8;
		}
		tmp=tmp+buff[i];
		printf("%08X\n",tmp);
    }
    infile.close();


    return 0;
}

