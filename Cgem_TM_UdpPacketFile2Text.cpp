#include <iostream>
#include<iomanip>  /*set format output with para*/
#include<fstream>
#include<string>

using namespace std;

/* This Cpp is used to read   TM UDP packet in a bin_file,
   not in  BESIII record format.
   g++ -std=c++11 -o Cgem_TM_UdpPacketFile2Text Cgem_TM_UdpPacketFile2Text.cpp
*/

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
    unsigned int S_GEMROC_ID;
    long long UDP_packet_count;

};

class BinFileRead
{
public:
    BinFileRead(string fname):inFileName(fname){}

    int BinToText()
    {
        bool udpLoop = true;
        infile.open(inFileName,ifstream::binary);
        txtfile.open(inFileName+"_bin2txt.txt");
        txtfile<<left;
        if(!infile)
        {
            cerr<<"cannot open file."<<endl;
            return -1;
        }
        nUdpPacket=0;
        while(udpLoop && !infile.fail())
        {
            if(CheckAndExtractPara()==0)
                PrintToText();
            else
            {
                udpLoop=false;
                return -1;
            }
            nUdpPacket++;
        }
        txtfile.close();
        infile.close();
        return 0;
    }
    int CheckAndExtractPara()
    {
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
            for(j=0;j<3;j++)
            {
                tmp=tmp|buff[i+j];
                tmp=tmp<<8;
            }
            tmp=tmp|buff[i+j];
            intbuff[i>>2]=tmp;
        }

        if(intbuff[0]>>29 != 6)
        {
            cout<<"packet number = "<<nUdpPacket<<", header flag error."<<endl;
            return -1;
        }
        tmp_para.STATUS_BITS=(intbuff[0]>>26)&0x7;

        tmp_para.LOCAL_L1_COUNT=intbuff[0]&0x3ffffff;
        tmp_para.LOCAL_L1_COUNT=tmp_para.LOCAL_L1_COUNT<<6;
        tmp_para.LOCAL_L1_COUNT=tmp_para.LOCAL_L1_COUNT|(intbuff[1]>>24 & 0x3f);
        tmp_para.HIT_COUNT=intbuff[1]>>16 & 0xff;
        tmp_para.Timestamp=intbuff[1]&0xffff;

        nBytes=(tmp_para.HIT_COUNT<<3)+16;
        infile.read((char*)(buff+8),nBytes);
        if(infile.fail())
        {
            cerr<<"cannot read udp data and tailer,"<<" packet number = "<<nUdpPacket<<endl;
            return -1;
        }
        ChangeByteOrder(buff+8,nBytes);
        nBytes=nBytes+8;//total size of a UDP packet from one ROC

        for(int i=8;i<nBytes;i=i+4)
        {
            tmp=0;
            int j;
            for(j=0;j<3;j++)
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
        tmp_para.CH_ID=intbuff[idx]>>18&0x3f;
        tmp_para.LAST_COUNT_WORD_FROM_TIGER_DATA=intbuff[idx]&0x3ffff;
        idx++;
        if(intbuff[idx]>>28 !=4)
        {
            cout<<"packet number = "<<nUdpPacket<<", udp sequence flag error."<<endl;
            return -1;
        }
        tmp_para.S_GEMROC_ID=intbuff[idx]>>20&0x1f;
        tmp_para.UDP_packet_count=intbuff[idx]&0xfffff;
        idx++;
        tmp_para.UDP_packet_count=tmp_para.UDP_packet_count<<28;
        tmp_para.UDP_packet_count=tmp_para.UDP_packet_count|intbuff[idx]&0xfffffff;
        return 0;
    }

    void PrintToText()
    {
        txtfile<<setfill('*')<<setw(72)<<"*"<<setfill(' ')<<endl;
        txtfile<<"###UDP packet NO.= "<<nUdpPacket<<"###"<<endl;
        txtfile<<"Header:"<<endl;
        txtfile<<"        STATUS_BITS= "<<setw(12)<<tmp_para.STATUS_BITS<<"LOCAL_L1_COUNT= "<<setw(12)<<tmp_para.LOCAL_L1_COUNT<<endl;
        txtfile<<"        HIT_COUNT  = "<<setw(12)<<tmp_para.HIT_COUNT<<"Timestamp= "<<setw(12)<<tmp_para.Timestamp<<endl;
        txtfile<<"Data:"<<endl;
        for(int i=2;i<2+tmp_para.HIT_COUNT*2;i=i+2)
        {
            tmp_para.D_TIGER_ID=intbuff[i]>>27&0x7;
            tmp_para.LAST_TIGER_FRAME_NUMBER=intbuff[i]>>24&0x7;
            tmp_para.RAW_DATA=intbuff[i]&0xffffff;
            tmp_para.RAW_DATA=tmp_para.RAW_DATA<<30;
            tmp_para.RAW_DATA=tmp_para.RAW_DATA|(intbuff[i+1]&0x3fffffff);
            txtfile<<"        TIGER ID= "<<setw(6)<<tmp_para.D_TIGER_ID<<" LAST_TIGER_FRAME_NUMBER= "
            <<setw(6)<<tmp_para.LAST_TIGER_FRAME_NUMBER<<"RAW DATA= "
            <<hex<<setfill('0')<<setw(14)<<tmp_para.RAW_DATA<<setfill(' ')<<dec<<endl;
        }
        txtfile<<"Tailer:"<<endl;
        txtfile<<"        LOCAL_L1_FRAMENUM="<<setw(8)<<tmp_para.LOCAL_L1_FRAMENUM<<"GEMROC ID= "<<setw(12)<<tmp_para.GEMROC_ID<<endl;
        txtfile<<"        TIGER ID         ="<<setw(8)<<tmp_para.T_TIGER_ID<<"CH_ID= "<<setw(12)<<tmp_para.CH_ID<<endl;
        txtfile<<"Sequence:"<<endl;
        txtfile<<"        GEMROC ID        ="<<setw(8)<<tmp_para.S_GEMROC_ID
        <<"UDP_packet_count= "<<setw(15)<<tmp_para.UDP_packet_count<<endl;
        txtfile<<endl;
    }
    int BinToHex()
    {
        infile.open(inFileName,ifstream::binary);
        hexfile.open(inFileName+"_bin2hex.txt");
        if(!infile)
        {
            cerr<<"cannot open file."<<endl;
            return -1;
        }
        hexfile<<hex;
        while(!infile.fail())
        {
           infile.read((char*)buff,8);
            if(infile.fail())
            {
                cerr<<"File end."<<endl;
                return -1;
            }
            ChangeByteOrder(buff,8);
            for(int i=0;i<8;i=i+4)
            {
                tmp=0;
                int j;
                for(j=0;j<3;j++)
                {
                    tmp=tmp|buff[i+j];
                    tmp=tmp<<8;
                }
                tmp=tmp|buff[i+j];
                intbuff[i>>2]=tmp;
            }
            hexfile<<"0x"<<setfill('0')<<setw(8)<<intbuff[0]
            <<' '<<setfill('0')<<setw(8)<<intbuff[1]<<endl;
        }
        hexfile.close();
        infile.close();
        return 0;
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

protected:

private:
    int           nUdpPacket;/*记录检查到第几个roc UDP包了，便于出错提示*/
    int           nBytes;  /*临时变量*/
    unsigned int  tmp;
    int           idx;  /*index of inbuff*/
    unsigned char buff[2560];
    unsigned int  intbuff[640];
    para          tmp_para; /*info of a UDP packet*/
    string        inFileName;
    ifstream      infile;
    ofstream      txtfile;
    ofstream      hexfile;
};




int main(int argc, char** argv)
{
    cout << "Defaulf input filename: cgem_TM_UdpPacket.dat." << endl;
    string fname;
    fname="cgem_TM_UdpPacket.dat";
    if(argc==2)
	fname=argv[1];
    BinFileRead b(fname);
   // b.BinToHex();
    b.BinToText();
    return 0;
}
