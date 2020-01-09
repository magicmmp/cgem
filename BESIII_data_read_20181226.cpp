#include <iostream>
#include<iomanip>  /*set format output with para*/
#include<fstream>
#include<sstream>
#include<string>
#include<vector>
using namespace std;

struct para
{
    /*Header*/
    unsigned int STATUS_BITS;/*is 0*/
    unsigned int LOCAL_L1_COUNT; /*Increment by 1*/
    unsigned int HIT_COUNT;
    unsigned int Timestamp;
    /*1个HIT的data*/
    unsigned int D_TIGER_ID;
    unsigned int LAST_TIGER_FRAME_NUMBER;
    long long RAW_DATA;
    /*Tailer*/
    unsigned int LOCAL_L1_FRAMENUM;
    unsigned int T_TIGER_ID; /*tailer TIGER_ID*/
    unsigned int GEMROC_ID;
    unsigned int CH_ID;
    unsigned int LAST_COUNT_WORD_FROM_TIGER_DATA;
    /*UDP Sequence Counter*/
    long long UDP_packet_count;
    unsigned int S_GEMROC_ID;
};


class besFileRead
{
public:
    besFileRead(string fname,bool print_para):inFileName(fname)
    {
		printFlag=print_para;
        eventCount=0;
		nUdpPacket=0;
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


        while(! infile.eof())
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
				if(printFlag)
				{
                    outfile<<setw(8)<<" "<<"DATA SEPARATOR RECORD:"<<endl;
                    outfile<<showbase<<hex<<setw(8)<<" "<<"marker: "<<setw(16)<<intbuff[0]<<noshowbase<<dec<<endl;
                    outfile<<setw(8)<<" "<<"record size: "<<setw(16)<<intbuff[1]<<" word"<<endl;
                    outfile<<setw(8)<<" "<<"data_block_number: "<<setw(16)<<intbuff[2]<<endl;
                    outfile<<setw(8)<<" "<<"data_block_size: "<<setw(16)<<intbuff[3]<<endl;
				}
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
						if(printFlag)
						{
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
						}
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
					cout<<endl<<"Total UDP packets = "<<nUdpPacket<<endl;



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
			if(printFlag)
			{
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
			}
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
			if(printFlag)
			{
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
			}
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
			if(printFlag)
			{
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
			}
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
                infile.read((char*)(buff+8),28);
                for(int i=8;i<36;i=i+4)
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
                auto dataPos=infile.tellg();/*CGEM UDP包开始位置，先读完状态值和尾部再跳回读数据*/
                unsigned int udpBytes=(rodSize<<2)-52;
                infile.seekg(udpBytes,fstream::cur);
                infile.read((char*)(buff+36),16);
                for(int i=36;i<52;i=i+4)
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
                auto endPos=infile.tellg();
			if(printFlag)
			{
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
                outfile<<setw(48)<<" "<<setw(48)<<"status    value: "<<setw(16)<<intbuff[9]<<endl;
                outfile<<setw(48)<<" "<<setw(48)<<"status elements: "<<setw(16)<<intbuff[10]<<endl;
                outfile<<setw(48)<<" "<<setw(48)<<"data   elements: "<<setw(16)<<intbuff[11]<<endl;
                outfile<<setw(48)<<" "<<setw(48)<<"status position: "<<setw(16)<<intbuff[12]<<endl;
			}
                infile.seekg(dataPos); /*开始读udp包*/
                BinToText(udpBytes);
                infile.seekg(endPos);
            }
            else  /*is not rod header*/
            {
                cout<<"error,not a ROD header!"<<endl;
            }
        }

    }

   void BinToText(int udpBytes)
    {
        bool udpLoop = true;
        while(udpLoop && !infile.fail() && udpBytes>0)
        {
            if(CheckAndExtractPara(udpBytes)==0)
            {
               PrintToText();
               udpBytes=udpBytes-(tmp_para.HIT_COUNT<<3)-24;
               nUdpPacket++;
            }
            else
            {
                udpLoop=false;
            }
        }
    }
    int CheckAndExtractPara(int nBytesLeft)
    {
        if(nBytesLeft<24)
        {
            cerr<<"UDP packet length < 24,error."<<endl;
            return -1;
        }
        infile.read((char*)buff,8);
        if(infile.fail())
        {
            cerr<<"cannot read udp header,"<<" Number of packet read = "<<nUdpPacket<<endl;
            return -1;
        }
		ChangeByteOrder(buff,8);

        for(int i=0;i<8;i=i+4)
        {
            tmp=0;
            int j;
            for(j=3;j>0;j--)
            {
                tmp=tmp|buff[i+j];
                tmp=tmp<<8;
            }
            tmp=tmp|buff[i+j];
            intbuff[i>>2]=tmp;
        }

        if(intbuff[0]>>29 != 6)
        {
            cout<<"intbuff[0] = "<<intbuff[0]<<", header flag error."<<endl;
            return -1;
        }
        tmp_para.STATUS_BITS=(intbuff[0]>>26)&0x7;

        tmp_para.LOCAL_L1_COUNT=intbuff[0]&0x3ffffff;
        tmp_para.LOCAL_L1_COUNT=tmp_para.LOCAL_L1_COUNT<<6;
        tmp_para.LOCAL_L1_COUNT=tmp_para.LOCAL_L1_COUNT|(intbuff[1]>>24 & 0x3f);
        tmp_para.HIT_COUNT=intbuff[1]>>16 & 0xff;
        tmp_para.Timestamp=intbuff[1]&0xffff;

        int nBytes=(tmp_para.HIT_COUNT<<3)+16;
        infile.read((char*)(buff+8),nBytes);
        if(infile.fail())
        {
            cerr<<"cannot read udp data and tailer,"<<" packet number = "<<nUdpPacket<<endl;
            return -1;
        }
		ChangeByteOrder(buff+8,nBytes);
        nBytes=nBytes+8;
        for(int i=8;i<nBytes;i=i+4)
        {
            tmp=0;
            int j;
            for(j=3;j>0;j--)
            {
                tmp=tmp|buff[i+j];
                tmp=tmp<<8;
            }
            tmp=tmp|buff[i+j];
            intbuff[i>>2]=tmp;
        }
        idx=(nBytes>>2)-4; /*intbuff中tailer的开始下标*/


        if(intbuff[idx]>>29 !=7)
        {
            cout<<"packet number = "<<nUdpPacket<<", tailer flag error."<<endl;
            return -1;
        }
        tmp_para.LOCAL_L1_FRAMENUM=(intbuff[idx]>>5)&0xffffff;
        tmp_para.GEMROC_ID=intbuff[idx]&0x1f;
        idx++;
        tmp_para.T_TIGER_ID=intbuff[idx]>>27 & 0x7;
        if((intbuff[idx]>>24&0x7) != (tmp_para.LOCAL_L1_COUNT&0x7))
        {
            cout<<"packet number = "<<nUdpPacket<<", intbuff[idx]>>24&0x7) !=tmp_para.LOCAL_L1_COUNT&0x7, error."<<endl;
            return -1;
        }
        tmp_para.CH_ID=intbuff[idx]>>18&0x3f;
        tmp_para.LAST_COUNT_WORD_FROM_TIGER_DATA=intbuff[idx]&0x3ffff;
        idx++;
        if(intbuff[idx]>>28 !=4)
        {
            cout<<"packet number = "<<nUdpPacket<<", udp sequence flag1 error."<<endl;
            return -1;
        }
        tmp_para.S_GEMROC_ID=intbuff[idx]>>20&0x1f;
        tmp_para.UDP_packet_count=intbuff[idx]&0xfffff;
        idx++;
        if(intbuff[idx]>>28)
        {
            cout<<"packet number = "<<nUdpPacket<<", udp sequence flag2 error."<<endl;
            return -1;
        }
        tmp_para.UDP_packet_count=tmp_para.UDP_packet_count<<28;
        tmp_para.UDP_packet_count=tmp_para.UDP_packet_count|intbuff[idx]&0xfffffff;
        return 0;
    }

    void PrintToText()
    {
		int setW;
		if(printFlag)
			setW=56;
		else
			setW=0;
        outfile<<setw(setW)<<" "<<setfill('*')<<setw(72)<<"*"<<setfill(' ')<<endl;
		outfile<<setw(setW)<<" "<<"###UDP packet NO.= "<<nUdpPacket<<"###"<<endl;
        outfile<<setw(setW)<<" "<<"STATUS_BITS= "<<setw(12)<<tmp_para.STATUS_BITS<<"LOCAL_L1_COUNT= "<<setw(12)<<tmp_para.LOCAL_L1_COUNT<<endl;
        outfile<<setw(setW)<<" "<<"HIT_COUNT  = "<<setw(12)<<tmp_para.HIT_COUNT<<"Timestamp= "<<setw(12)<<tmp_para.Timestamp<<endl;
        outfile<<setw(setW)<<" "<<"DATA :"<<endl;
        for(int i=2;i<2+tmp_para.HIT_COUNT*2;i=i+2)
        {
            tmp_para.D_TIGER_ID=intbuff[i]>>27&0x7;
            tmp_para.LAST_TIGER_FRAME_NUMBER=intbuff[i]>>24&0x7;
            tmp_para.RAW_DATA=intbuff[i]&0xffffff;
            tmp_para.RAW_DATA=tmp_para.RAW_DATA<<30;
            tmp_para.RAW_DATA=tmp_para.RAW_DATA|(intbuff[i+1]&0x3fffffff);
            outfile<<setw(setW+2)<<" "<<"TIGER ID= "<<setw(6)<<tmp_para.D_TIGER_ID<<" LAST_TIGER_FRAME_NUMBER= "
            <<setw(6)<<tmp_para.LAST_TIGER_FRAME_NUMBER<<"RAW DATA= "
            <<hex<<setfill('0')<<setw(14)<<tmp_para.RAW_DATA<<setfill(' ')<<dec<<endl;
        }
        outfile<<setw(setW)<<" "<<"LOCAL_L1_FRAMENUM="<<setw(8)<<tmp_para.LOCAL_L1_FRAMENUM<<"GEMROC ID= "<<setw(12)<<tmp_para.GEMROC_ID<<endl;
        outfile<<setw(setW)<<" "<<"TIGER ID         ="<<setw(8)<<tmp_para.T_TIGER_ID<<"CH_ID= "<<setw(12)<<tmp_para.CH_ID<<endl;
        outfile<<setw(setW)<<" "<<"GEMROC ID        ="<<setw(8)<<tmp_para.S_GEMROC_ID
        <<"UDP_packet_count= "<<setw(15)<<tmp_para.UDP_packet_count<<endl;
        outfile<<endl;
    }
	
	

    void udpData(unsigned int idx1,unsigned int idx2)
    {
        for(int i=idx1;i<idx2;i=i+1)
        {
            outfile<<hex;
            outfile<<setw(8)<<intbuff[i];
            outfile<<dec<<" : "<<intbuff[i]<<endl;
        }
    }

	void ChangeByteOrder(unsigned char*buff,int buffLen)
	{
    	int i,j;
    	unsigned char ch;
    	for(i=0;i<buffLen;i=i+8)
    	{
        	for(j=0;j<4;j++)
        	{
            	ch=buff[i+j];
            	buff[i+j]=buff[i+7-j];
            	buff[i+7-j]=ch;
        	}
    	}
	}


private:
    int           nUdpPacket;/*记录检查到一个ROD中的第几个roc UDP包了，便于出错提示*/
    para          tmp_para;
    unsigned int  eventCount;
    unsigned int  tmp;
    int           idx;  /*index of inbuff*/
    unsigned char buff[10240];
    unsigned int  intbuff[2560];
	bool 		  printFlag;
    string        inFileName;
    ifstream      infile;
    ofstream      outfile;

};

int main(int argc, char** argv)
{
	cout << "if input para =1 ,print more,else UDP packet only." << endl;
	bool print_para;
	if(argc==2 && atoi(argv[1])==1)
        print_para=true;
    else
        print_para=false;

    string inFileName;
    cout<<"Input a file name:"<<endl;
    cin>>inFileName;

  //  besFileRead b("bes.raw");
 //   besFileRead b("run_0010013_FakePhysics_SFO-1.raw");
    besFileRead b(inFileName,print_para);
    b.fileRead();
    return 0;
}
