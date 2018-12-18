#include <iostream>
#include<iomanip>  /*set format output with para*/
#include<fstream>
#include<sstream>
#include<string>
#include<vector>
using namespace std;

class besFileRead
{
public:
    besFileRead(string fname):inFileName(fname)
    {

        eventCount=0;
    }
    int fileRead()
    {
        infile.open(inFileName,ifstream::binary);
        outfile.open(inFileName+"_outPut.txt");
        outfile<<left;
        if(!infile)
        {
            cerr<<"cannot open file."<<endl;
            return -1;
        }
        infile.read((char*)buff,4);
        tmp=0;
        for(int i=3;i>0;i--)
        {
            tmp=tmp|buff[i];
            tmp=tmp<<8;
        }
        tmp=tmp|buff[0];
        if(tmp!=0x1234aaaa)
        {
            cout<<"File start record marker != 0x1234aaaa!"<<endl;
            return -1;
        }

        infile.seekg(-4,fstream::end);
        infile.read((char*)buff,4);
        tmp=0;
        for(int i=3;i>0;i--)
        {
            tmp=tmp|buff[i];
            tmp=tmp<<8;
        }
        tmp=tmp|buff[0];
        if(tmp!=0x1234eeee)
        {
            cout<<"File end record marker != 0x1234eeee!"<<endl;
            return -1;
        }
        outfile<<"FILE START RECORD :"<<endl;
        infile.seekg(0,fstream::beg);
        infile.read((char*)buff,32);
        for(int i=0;i<32;i=i+4)
        {
            tmp=0;
            for(int j=3;j>0;j--)
            {
                tmp=tmp|buff[i+j];
                tmp=tmp<<8;
            }
            tmp=tmp|buff[i];
            intbuff[i>>2]=tmp;
        }
        outfile<<showbase<<hex<<"marker: "<<intbuff[0]<<noshowbase<<dec<<endl;
        outfile<<"record size: "<<intbuff[1]<<" word"<<endl;
        outfile<<"version: "<<intbuff[2]<<endl;
        outfile<<"file_number: "<<intbuff[3]<<endl;
        outfile<<"date: "<<intbuff[4]<<endl;
        outfile<<"time: "<<intbuff[5]<<endl;
        outfile<<"sizeLimit_dataBlocks: "<<intbuff[6]<<endl;
        outfile<<"max file size: "<<intbuff[5]<<" Mb"<<endl;

        infile.read((char*)buff,8);
        for(int i=0;i<8;i=i+4)
        {
            tmp=0;
            for(int j=3;j>0;j--)
            {
                tmp=tmp|buff[i+j];
                tmp=tmp<<8;
            }
            tmp=tmp|buff[i];
            intbuff[i>>2]=tmp;
        }
        outfile<<"File name string "<<endl;
        outfile<<showbase<<hex<<"marker: "<<intbuff[0]<<noshowbase<<dec<<endl;
        outfile<<"name length: "<<intbuff[1]<<" Bytes"<<endl;
        if(intbuff[1]%4)
            intbuff[1]=intbuff[1]+4-(intbuff[1]%4);
        infile.read((char*)buff,intbuff[1]);
        string sfname((char*)buff,intbuff[1]);
        outfile<<"File name: "<<sfname<<endl;

        infile.read((char*)buff,4);
        tmp=0;
        for(int i=3;i>0;i--)
        {
            tmp=tmp|buff[i];
            tmp=tmp<<8;
        }
        tmp=tmp|buff[0];
        outfile<<"file tag length: "<<tmp<<" Bytes"<<endl;
        if(tmp%4)
            tmp=tmp-(tmp%4)+4;
        infile.read((char*)buff,tmp);
        string stag((char*)buff,tmp);
        outfile<<"tag name: "<<stag<<endl;

        outfile<<"run_parameters_record"<<endl;
        infile.read((char*)buff,36);
        for(int i=0;i<36;i=i+4)
        {
            tmp=0;
            for(int j=3;j>0;j--)
            {
                tmp=tmp|buff[i+j];
                tmp=tmp<<8;
            }
            tmp=tmp|buff[i];
            intbuff[i>>2]=tmp;
        }
        outfile<<showbase<<hex<<"marker: "<<setw(16)<<intbuff[0]<<noshowbase<<dec<<endl;
        outfile<<"record size: "<<setw(16)<<intbuff[1]<<" word"<<endl;
        outfile<<"run_number: "<<setw(16)<<intbuff[2]<<endl;
        outfile<<"max_events: "<<setw(16)<<intbuff[3]<<endl;
        outfile<<"rec_enable: "<<setw(16)<<intbuff[4]<<endl;
        outfile<<"trigger_type: "<<setw(16)<<intbuff[5]<<endl;
        outfile<<"detector_mask: "<<setw(16)<<intbuff[6]<<endl;
        outfile<<"beam_type: "<<setw(16)<<intbuff[7]<<endl;
        outfile<<"beam_energy: "<<setw(16)<<intbuff[8]<<endl;


        while(! infile.eof() && eventCount<100)
        {
            infile.read((char*)buff,8);
            if(infile.fail())
            {

                cout<<"read data_separator_record fail."<<endl;
                    cout<<boolalpha;
                    cout<<"Before file end:"<<endl;
                    cout<<"infile.eof() = "<<infile.eof()<<endl;
                    cout<<"infile.fail()= "<<infile.fail()<<endl;
                    cout<<"infile.good()= "<<infile.good()<<endl;
                    cout<<noboolalpha;
                return -1;
            }
            else
            {
                for(int i=0;i<8;i=i+4)
                {
                    tmp=0;
                    for(int j=3;j>0;j--)
                    {
                        tmp=tmp|buff[i+j];
                        tmp=tmp<<8;
                    }
                    tmp=tmp|buff[i];
                    intbuff[i>>2]=tmp;
                }
                if(intbuff[0]==0x1234cccc)  /*is data_separator_record*/
                {
                    infile.read((char*)(buff+8),8);
                    for(int i=8;i<16;i=i+4)
                    {
                        tmp=0;
                        for(int j=3;j>0;j--)
                        {
                            tmp=tmp|buff[i+j];
                            tmp=tmp<<8;
                        }
                        tmp=tmp|buff[i];
                        intbuff[i>>2]=tmp;
                    }
                    outfile<<setw(8)<<" "<<"DATA SEPARATOR RECORD:"<<endl;
                    outfile<<showbase<<hex<<setw(8)<<" "<<"marker: "<<setw(16)<<intbuff[0]<<noshowbase<<dec<<endl;
                    outfile<<setw(8)<<" "<<"record size: "<<setw(16)<<intbuff[1]<<" word"<<endl;
                    outfile<<setw(8)<<" "<<"data_block_number: "<<setw(16)<<intbuff[2]<<endl;
                    outfile<<setw(8)<<" "<<"data_block_size: "<<setw(16)<<intbuff[3]<<endl;

                    infile.read((char*)buff,12);
                    if(infile.fail())
                    {

                        cout<<"read FullEventFragment fail."<<endl;
                        return -1;
                    }
                    else
                    {
                        for(int i=0;i<12;i=i+4)
                        {
                            tmp=0;
                            for(int j=3;j>0;j--)
                            {
                                tmp=tmp|buff[i+j];
                                tmp=tmp<<8;
                            }
                            tmp=tmp|buff[i];
                            intbuff[i>>2]=tmp;
                        }
                        if(intbuff[0]==0xAA1234AA)  /*Full event header*/
                        {
                            infile.read((char*)(buff+12),intbuff[2]*4-12);
                            for(int i=12;i<intbuff[2]*4;i=i+4)
                            {
                                tmp=0;
                                for(int j=3;j>0;j--)
                                {
                                    tmp=tmp|buff[i+j];
                                    tmp=tmp<<8;
                                }
                                tmp=tmp|buff[i];
                                intbuff[i>>2]=tmp;
                            }
                            outfile<<setw(16)<<" "<<"FULL EVENT "<<eventCount<<endl;
                            outfile<<showbase<<hex<<setw(16)<<" "<<setw(48)<<"Header marker: "<<setw(16)<<intbuff[0]<<noshowbase<<dec<<endl;
                            outfile<<setw(16)<<" "<<setw(48)<<"total Fragment size: "<<setw(16)<<intbuff[1]<<" word"<<endl;
                            outfile<<setw(16)<<" "<<setw(48)<<"Header size: "<<setw(16)<<intbuff[2]<<endl;
                            outfile<<setw(16)<<" "<<setw(48)<<"version Number: "<<setw(16)<<intbuff[3]<<endl;
                            outfile<<setw(16)<<" "<<setw(48)<<"Source identifier,Sub-detectorID: "
                                   <<showbase<<hex<<setw(16)<<((intbuff[4]>>16)&0xff)<<"  ,module ID: "<<setw(16)<<(intbuff[4]&0xffff)
                                   <<noshowbase<<dec<<endl;
                            outfile<<setw(16)<<" "<<setw(48)<<"Number of status elements: "<<setw(16)<<intbuff[5]<<endl;
                            int eidx; /* intbuff index with event data*/
                            for(eidx=6;eidx<6+intbuff[5];eidx++)
                                outfile<<setw(16)<<" "<<setw(48)<<"status elements "<<eidx-6<<": "<<setw(16)<<intbuff[eidx]<<endl;
                            outfile<<setw(16)<<" "<<setw(48)<<"Number of specific elements: "<<setw(16)<<intbuff[eidx]<<endl;

                            outfile<<setw(16)<<" "<<setw(48)<<"event build time: "<<setw(16)<<intbuff[++eidx]<<endl;
                            outfile<<setw(16)<<" "<<setw(48)<<"Global event id: "<<setw(16)<<intbuff[++eidx]<<endl;

                            outfile<<setw(16)<<" "<<setw(48)<<"run number: "<<setw(16)<<intbuff[++eidx]<<endl;
                            outfile<<setw(16)<<" "<<setw(48)<<"Level 1 ID: "<<setw(16)<<intbuff[++eidx]<<endl;
                            outfile<<setw(16)<<" "<<setw(48)<<"default: "<<setw(16)<<intbuff[++eidx]<<endl;
                            outfile<<setw(16)<<" "<<setw(48)<<"default: "<<setw(16)<<intbuff[++eidx]<<endl;
                            outfile<<setw(16)<<" "<<setw(48)<<"event mark1: "<<setw(16)<<intbuff[++eidx]<<endl;
                            outfile<<setw(16)<<" "<<setw(48)<<"event mark2: "<<setw(16)<<intbuff[++eidx]<<endl;
                            outfile<<setw(16)<<" "<<setw(48)<<"event mark3: "<<setw(16)<<intbuff[++eidx]<<endl;
                            outfile<<setw(16)<<" "<<setw(48)<<"event mark4: "<<setw(16)<<intbuff[++eidx]<<endl;

                            subDectector();
			    eventCount++;



                        }
                        else
                        {
                            /*如果不是事例头*/
                            cout<<"error,Full event header not found."<<endl;
                            return -1;
                        }

                    }

                }
                else if(intbuff[0]==0x1234dddd)
                {
                    /*file end*/

                    infile.read((char*)(buff+8),intbuff[1]*4-8);

                    for(int i=8;i<intbuff[1]*4;i=i+4)
                    {
                        tmp=0;
                        for(int j=3;j>0;j--)
                        {
                            tmp=tmp|buff[i+j];
                            tmp=tmp<<8;
                        }
                        tmp=tmp|buff[i];
                        intbuff[i>>2]=tmp;
                    }
                    outfile<<"FILE END :"<<endl;
                    outfile<<showbase<<hex<<setw(8)<<" "<<"marker: "<<setw(16)<<intbuff[0]<<noshowbase<<dec<<endl;
                    outfile<<"record size: "<<setw(16)<<intbuff[1]<<" word"<<endl;
                    outfile<<"file closed date: "<<setw(16)<<intbuff[2]<<endl;
                    outfile<<"file closed time: "<<setw(16)<<intbuff[3]<<endl;

                    outfile<<"events_in_file: "<<setw(16)<<intbuff[1]<<" word"<<endl;
                    outfile<<"data_in_file: "<<setw(16)<<intbuff[2]<<endl;
                    outfile<<"events_in_run: "<<setw(16)<<intbuff[3]<<endl;
                    outfile<<"data_in_run: "<<setw(16)<<intbuff[1]<<" word"<<endl;
                    outfile<<"status: "<<setw(16)<<intbuff[2]<<endl;
                    outfile<<"end_marker: "<<setw(16)<<intbuff[3]<<endl;



                }
                else
                {
                    cout<<"format error!"<<endl;
                    cout<<showbase<<hex<<setw(8)<<" "<<"marker: "<<setw(16)<<intbuff[0]<<noshowbase<<dec<<endl;
                    cout<<"record size: "<<setw(16)<<intbuff[1]<<" word"<<endl;

                    return -1;
                }

            }
        }




        outfile.close();
        infile.close();
        return 0;
    }


protected:
    void subDectector()
    {
        bool subLoop=true;

        while(subLoop && infile.good())
        {
            auto subPos=infile.tellg();
            infile.read((char*)buff,12);
            for(int i=0;i<12;i=i+4)
            {
                tmp=0;
                for(int j=3;j>0;j--)
                {
                    tmp=tmp|buff[i+j];
                    tmp=tmp<<8;
                }
                tmp=tmp|buff[i];
                intbuff[i>>2]=tmp;
            }
            if(intbuff[0]==0xBB1234BB)  /*sub-dectector header*/
            {
                infile.read((char*)(buff+12),intbuff[2]*4-12);
                for(int i=12;i<intbuff[2]*4;i=i+4)
                {
                    tmp=0;
                    for(int j=3;j>0;j--)
                    {
                        tmp=tmp|buff[i+j];
                        tmp=tmp<<8;
                    }
                    tmp=tmp|buff[i];
                    intbuff[i>>2]=tmp;
                }
                outfile<<setw(24)<<" "<<"Sub-Dectector:"<<endl;
                outfile<<showbase<<hex<<setw(24)<<" "<<setw(48)<<"Header marker: "<<setw(16)<<intbuff[0]<<noshowbase<<dec<<endl;
                outfile<<setw(24)<<" "<<setw(48)<<"Sub-detector Fragment size: "<<setw(16)<<intbuff[1]<<" word"<<endl;
                outfile<<setw(24)<<" "<<setw(48)<<"Header size: "<<setw(16)<<intbuff[2]<<endl;
                outfile<<setw(24)<<" "<<setw(48)<<"version Number: "<<setw(16)<<intbuff[3]<<endl;
                outfile<<setw(24)<<" "<<setw(48)<<"Source identifier,Sub-detectorID: "
                        <<showbase<<hex<<setw(16)<<((intbuff[4]>>16)&0xff)<<"  ,module ID: "<<setw(16)<<(intbuff[4]&0xffff)
                        <<noshowbase<<dec<<endl;
                outfile<<setw(24)<<" "<<setw(48)<<"Number of status elements: "<<setw(16)<<intbuff[5]<<endl;

                for(idx=6;idx<6+intbuff[5];idx++)
                    outfile<<setw(24)<<" "<<setw(24)<<"status elements "<<idx-6<<" : "<<setw(16)<<intbuff[idx]<<endl;
                outfile<<setw(24)<<" "<<setw(48)<<"Number of specific elements: "<<setw(16)<<intbuff[idx]<<endl;
                outfile<<setw(24)<<" "<<setw(48)<<"HV status: "<<setw(16)<<intbuff[++idx]<<endl;
                ROS();


            }
            else  /*is not sub-dectector header*/
            {
                infile.seekg(subPos);
                subLoop=false;
            }

        }

    }
    void ROS()
    {
        bool rosLoop=true;

        while(rosLoop && infile.good())
        {
            auto rosPos=infile.tellg();
            infile.read((char*)buff,12);
            for(int i=0;i<12;i=i+4)
            {
                tmp=0;
                for(int j=3;j>0;j--)
                {
                    tmp=tmp|buff[i+j];
                    tmp=tmp<<8;
                }
                tmp=tmp|buff[i];
                intbuff[i>>2]=tmp;
            }
            if(intbuff[0]==0xCC1234CC)  /*ros header*/
            {
                infile.read((char*)(buff+12),intbuff[2]*4-12);
                for(int i=12;i<intbuff[2]*4;i=i+4)
                {
                    tmp=0;
                    for(int j=3;j>0;j--)
                    {
                        tmp=tmp|buff[i+j];
                        tmp=tmp<<8;
                    }
                    tmp=tmp|buff[i];
                    intbuff[i>>2]=tmp;
                }
                outfile<<setw(32)<<" "<<"ROS-ROS:"<<endl;
                outfile<<showbase<<hex<<setw(32)<<" "<<setw(48)<<"Header marker: "<<setw(16)<<intbuff[0]<<noshowbase<<dec<<endl;
                outfile<<setw(32)<<" "<<setw(48)<<"ROS Fragment size: "<<setw(16)<<intbuff[1]<<" word"<<endl;
                outfile<<setw(32)<<" "<<setw(48)<<"Header size: "<<setw(16)<<intbuff[2]<<endl;
                outfile<<setw(32)<<" "<<setw(48)<<"version Number: "<<setw(16)<<intbuff[3]<<endl;
                outfile<<setw(32)<<" "<<setw(48)<<"Source identifier,Sub-detectorID: "
                        <<showbase<<hex<<setw(16)<<((intbuff[4]>>16)&0xff)<<"  ,module ID: "<<setw(16)<<(intbuff[4]&0xffff)
                        <<noshowbase<<dec<<endl;
                outfile<<setw(32)<<" "<<setw(48)<<"Number of status elements: "<<setw(16)<<intbuff[5]<<endl;

                for(idx=6;idx<6+intbuff[5];idx++)
                    outfile<<setw(32)<<" "<<setw(24)<<"status elements "<<idx-6<<" : "<<setw(16)<<intbuff[idx]<<endl;
                outfile<<setw(32)<<" "<<setw(48)<<"Number of specific elements: "<<setw(16)<<intbuff[idx]<<endl;
                outfile<<setw(32)<<" "<<setw(48)<<"run number: "<<setw(16)<<intbuff[++idx]<<endl;
                outfile<<setw(32)<<" "<<setw(48)<<"default: "<<setw(16)<<intbuff[++idx]<<endl;
                outfile<<setw(32)<<" "<<setw(48)<<"trigger number: "<<setw(16)<<intbuff[++idx]<<endl;
                ROB();


            }
            else  /*is not ros header*/
            {
                infile.seekg(rosPos);
                rosLoop=false;
            }
        }

    }

    void ROB()
    {
        bool robLoop=true;

        while(robLoop && infile.good())
        {
            auto robPos=infile.tellg();
            infile.read((char*)buff,12);
            for(int i=0;i<12;i=i+4)
            {
                tmp=0;
                for(int j=3;j>0;j--)
                {
                    tmp=tmp|buff[i+j];
                    tmp=tmp<<8;
                }
                tmp=tmp|buff[i];
                intbuff[i>>2]=tmp;
            }
            if(intbuff[0]==0xdd1234dd)  /*rob header*/
            {
                infile.read((char*)(buff+12),intbuff[2]*4-12);
                for(int i=12;i<intbuff[2]*4;i=i+4)
                {
                    tmp=0;
                    for(int j=3;j>0;j--)
                    {
                        tmp=tmp|buff[i+j];
                        tmp=tmp<<8;
                    }
                    tmp=tmp|buff[i];
                    intbuff[i>>2]=tmp;
                }
                outfile<<setw(40)<<" "<<"ROB-ROB:"<<endl;
                outfile<<showbase<<hex<<setw(40)<<" "<<setw(48)<<"Header marker: "<<setw(16)<<intbuff[0]<<noshowbase<<dec<<endl;
                outfile<<setw(40)<<" "<<setw(48)<<"ROB Fragment size: "<<setw(16)<<intbuff[1]<<" word"<<endl;
                outfile<<setw(40)<<" "<<setw(48)<<"Header size: "<<setw(16)<<intbuff[2]<<endl;
                outfile<<setw(40)<<" "<<setw(48)<<"version Number: "<<setw(16)<<intbuff[3]<<endl;
                outfile<<setw(40)<<" "<<setw(48)<<"Source identifier,Sub-detectorID: "
                        <<showbase<<hex<<setw(16)<<((intbuff[4]>>16)&0xff)<<"  ,module ID: "<<setw(16)<<(intbuff[4]&0xffff)
                        <<noshowbase<<dec<<endl;
                outfile<<setw(40)<<" "<<setw(48)<<"Number of status elements: "<<setw(16)<<intbuff[5]<<endl;

                for(idx=6;idx<6+intbuff[5];idx++)
                    outfile<<setw(40)<<" "<<setw(24)<<"status elements "<<idx-6<<" : "<<setw(16)<<intbuff[idx]<<endl;
                outfile<<setw(40)<<" "<<setw(48)<<"Number of specific elements: "<<setw(16)<<intbuff[idx]<<endl;
                ROD(intbuff[1]-intbuff[2]);


            }
            else  /*is not ros header*/
            {
                infile.seekg(robPos);
                robLoop=false;
            }
        }

    }

    void ROD(unsigned int rodSize)
    {
        if(infile.good())
        {
            infile.read((char*)buff,8);
            for(int i=0;i<8;i=i+4)
            {
                tmp=0;
                for(int j=3;j>0;j--)
                {
                    tmp=tmp|buff[i+j];
                    tmp=tmp<<8;
                }
                tmp=tmp|buff[i];
                intbuff[i>>2]=tmp;
            }
            if(intbuff[0]==0xee1234ee)  /*rob header*/
            {
                unsigned int rodBytes=rodSize<<2;
                infile.read((char*)(buff+8),rodBytes-8);
                for(int i=8;i<rodBytes;i=i+4)
                {
                    tmp=0;
                    for(int j=3;j>0;j--)
                    {
                        tmp=tmp|buff[i+j];
                        tmp=tmp<<8;
                    }
                    tmp=tmp|buff[i];
                    intbuff[i>>2]=tmp;
                }
                outfile<<setw(48)<<" "<<"ROD-ROD:"<<endl;
                outfile<<showbase<<hex<<setw(48)<<" "<<setw(48)<<"Header marker: "<<setw(16)<<intbuff[0]<<noshowbase<<dec<<endl;
                outfile<<setw(48)<<" "<<setw(48)<<"ROD header size: "<<setw(16)<<intbuff[1]<<" word"<<endl;
                outfile<<setw(48)<<" "<<setw(48)<<"ROD total  size: "<<setw(16)<<rodSize<<" word"<<endl;


                outfile<<setw(48)<<" "<<setw(48)<<"version Number: "<<setw(16)<<intbuff[2]<<endl;
                outfile<<setw(48)<<" "<<setw(48)<<"Source identifier,Sub-detectorID: "
                        <<showbase<<hex<<setw(16)<<((intbuff[3]>>16)&0xff)<<"  ,module ID: "<<setw(16)<<(intbuff[3]&0xffff)
                        <<noshowbase<<dec<<endl;
                outfile<<setw(48)<<" "<<setw(48)<<"run number: "<<setw(16)<<intbuff[4]<<endl;
                outfile<<setw(48)<<" "<<setw(48)<<"trigger number: "<<setw(16)<<intbuff[5]<<endl;
                outfile<<setw(48)<<" "<<setw(48)<<"default: "<<setw(16)<<intbuff[6]<<endl;
                outfile<<setw(48)<<" "<<setw(48)<<"default: "<<setw(16)<<intbuff[7]<<endl;
                outfile<<setw(48)<<" "<<setw(48)<<"default: "<<setw(16)<<intbuff[8]<<endl;
                /*

                for(idx=6;idx<6+intbuff[5];idx++)
                    outfile<<setw(48)<<" "<<setw(24)<<"status elements "<<idx-6<<" : "<<setw(16)<<intbuff[idx]<<endl;
                outfile<<setw(48)<<" "<<setw(48)<<"Number of specific elements: "<<setw(16)<<intbuff[idx]<<endl;
                */
                udpData(buff,rodSize);
                outfile<<setw(48)<<" "<<setw(48)<<"status elements: "<<setw(16)<<intbuff[rodSize-3]<<endl;
                outfile<<setw(48)<<" "<<setw(48)<<"data   elements: "<<setw(16)<<intbuff[rodSize-2]<<endl;
                outfile<<setw(48)<<" "<<setw(48)<<"status position: "<<setw(16)<<intbuff[rodSize-1]<<endl;


            }
            else  /*is not rod header*/
            {
                cout<<"error,not a ROD header!"<<endl;
            }
        }

    }
    void udpData(unsigned char*p,unsigned int rodSizes)
    {
        for(int i=9;i<rodSizes;i=i+1)
	{
	    outfile<<hex;
            outfile<<setw(56)<<" "<<setfill('0')<<setw(8)<<intbuff[i]<<setfill(' ');
            outfile<<dec<<" : "<<intbuff[i]<<endl;
	}
    }




private:
    unsigned int  eventCount;
    unsigned int  tmp;
    int           idx;  /*index of inbuff*/
    unsigned char buff[10240];
    unsigned int  intbuff[2560];
    string        inFileName;
    ifstream      infile;
    ofstream      outfile;

};

int main()
{
    string inFileName;
    cout<<"Input a file name:"<<endl;
    cin>>inFileName;

  //  besFileRead b("bes.raw");
 //   besFileRead b("run_0010013_FakePhysics_SFO-1.raw");
    besFileRead b(inFileName);
    b.fileRead();
    return 0;
}
