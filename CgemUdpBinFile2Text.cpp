#include <iostream>
#include<iomanip>  /*set format output with para*/
#include<fstream>
#include<string>

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

        udpData(0,2);

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
        /*
        tmp_para.LOCAL_L1_COUNT=buff[0]&0x3;
        tmp_para.LOCAL_L1_COUNT=tmp_para.LOCAL_L1_COUNT<<8;
        tmp_para.LOCAL_L1_COUNT=tmp_para.LOCAL_L1_COUNT+buff[1];
        tmp_para.LOCAL_L1_COUNT=tmp_para.LOCAL_L1_COUNT<<8;
        tmp_para.LOCAL_L1_COUNT=tmp_para.LOCAL_L1_COUNT+buff[2];
        tmp_para.LOCAL_L1_COUNT=tmp_para.LOCAL_L1_COUNT<<8;
        tmp_para.LOCAL_L1_COUNT=tmp_para.LOCAL_L1_COUNT+buff[3];
        tmp_para.LOCAL_L1_COUNT=tmp_para.LOCAL_L1_COUNT<<6;
        tmp_para.LOCAL_L1_COUNT=tmp_para.LOCAL_L1_COUNT+(buff[4]&0x3f);
        tmp_para.HIT_COUNT=buff[5];
        tmp_para.Timestamp=buff[6];
        tmp_para.Timestamp=tmp_para.Timestamp<<8;
        tmp_para.Timestamp=tmp_para.Timestamp+buff[7];
        */


        nBytes=(tmp_para.HIT_COUNT<<3)+16;
        infile.read((char*)(buff+8),nBytes);
        if(infile.fail())
        {
            cerr<<"cannot read udp data and tailer,"<<" packet number = "<<nUdpPacket<<endl;
            return -1;
        }
        nBytes=nBytes+8;

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
            cout<<"packet number = "<<nUdpPacket<<", udp sequence flag error."<<endl;
            return -1;
        }
        tmp_para.S_GEMROC_ID=intbuff[idx]>>20&0x1f;
        tmp_para.UDP_packet_count=intbuff[idx]&0xfffff;
        idx++;
        if(intbuff[idx]>>28 !=4)
        {
            cout<<"packet number = "<<nUdpPacket<<", udp sequence flag error."<<endl;
            return -1;
        }
        tmp_para.UDP_packet_count=tmp_para.UDP_packet_count<<28;
        tmp_para.UDP_packet_count=tmp_para.UDP_packet_count|intbuff[idx]&0xfffffff;
        return 0;
    }

    void PrintToText()
    {
        txtfile<<setfill('*')<<setw(72)<<"*"<<setfill(' ')<<endl;
        txtfile<<"###UDP packet NO.= "<<nUdpPacket<<"###"<<endl;
        txtfile<<"STATUS_BITS= "<<setw(12)<<tmp_para.STATUS_BITS<<"LOCAL_L1_COUNT= "<<setw(12)<<tmp_para.LOCAL_L1_COUNT<<endl;
        txtfile<<"HIT_COUNT  = "<<setw(12)<<tmp_para.HIT_COUNT<<"Timestamp= "<<setw(12)<<tmp_para.Timestamp<<endl;
        txtfile<<"DATA :"<<endl;
        for(int i=2;i<2+tmp_para.HIT_COUNT*2;i=i+2)
        {
            tmp_para.D_TIGER_ID=intbuff[i]>>27&0x7;
            tmp_para.LAST_TIGER_FRAME_NUMBER=intbuff[i]>>24&0x7;
            tmp_para.RAW_DATA=intbuff[i]&0xffffff;
            tmp_para.RAW_DATA=tmp_para.RAW_DATA<<30;
            tmp_para.RAW_DATA=tmp_para.RAW_DATA|(intbuff[i+1]&0x3fffffff);
            txtfile<<"TIGER ID= "<<setw(6)<<tmp_para.D_TIGER_ID<<" LAST_TIGER_FRAME_NUMBER= "
            <<setw(6)<<tmp_para.LAST_TIGER_FRAME_NUMBER<<"RAW DATA= "
            <<hex<<setfill('0')<<setw(14)<<tmp_para.RAW_DATA<<setfill(' ')<<dec<<endl;
        }
        txtfile<<"LOCAL_L1_FRAMENUM="<<setw(8)<<tmp_para.LOCAL_L1_FRAMENUM<<"GEMROC ID= "<<setw(12)<<tmp_para.GEMROC_ID<<endl;
        txtfile<<"TIGER ID         ="<<setw(8)<<tmp_para.T_TIGER_ID<<"CH_ID= "<<setw(12)<<tmp_para.CH_ID<<endl;
        txtfile<<"GEMROC ID        ="<<setw(8)<<tmp_para.S_GEMROC_ID
        <<"UDP_packet_count= "<<setw(15)<<tmp_para.UDP_packet_count<<endl;
        txtfile<<endl;
    }
    int BinToHex()
    {
        infile.open(inFileName,ifstream::binary);
        hexfile.open(inFileName+"_bin2hex.txt");
        hexfile<<left;
        if(!infile)
        {
            cerr<<"cannot open file."<<endl;
            return -1;
        }
        while(!infile.fail())
        {
           infile.read((char*)buff,8);
            if(infile.fail())
            {
                cerr<<"File end."<<endl;
                return -1;
            }
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
             hexfile<<hex;
             hexfile<<setfill('0')<<setw(8)<<intbuff[0]<<setfill(' ');
             hexfile<<dec<<" : "<<intbuff[0]<<endl;

             hexfile<<hex;
             hexfile<<setfill('0')<<setw(8)<<intbuff[1]<<setfill(' ');
             hexfile<<dec<<" : "<<intbuff[1]<<endl;
        }
        hexfile.close();
        infile.close();
        return 0;
    }

    void udpData(unsigned int idx1,unsigned int idx2)
    {
        for(int i=idx1;i<idx2;i=i+1)
        {
            txtfile<<hex;
            txtfile<<setw(8)<<intbuff[i];
            txtfile<<dec<<" : "<<intbuff[i]<<endl;
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
	cout << "if input para =1 ,bin file to hex, else to txt." << endl;
    cout << "BinFileRead,now input a file name:" << endl;
    string fname;
    cin>>fname;
    BinFileRead b(fname);
	if(argc==2 && atoi(argv[1])==1)
    	b.BinToHex();
	else
    	b.BinToText();

    return 0;
}
