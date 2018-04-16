#include "BasicRW.h"


int Read_A32(int address,int * data)
{


	UINT32 addr=(UINT32)address;
	UINT32 testdata=0xffffffff;

	/*printf("OK\n");*/

	ReadFrmVme32(addr,VA32,&testdata);

	*data=testdata;

	printf("VME:: Read from Address<0x%08x>  Data:<0x%08x>\n",address,*data);

	return 0;
}

int Write_A32(int address,int data)
{

	UINT32 addr=(UINT32)address;
	UINT32 testdata=(UINT32)data;

	/*printf("W OK\n");*/

	WriteToVme32(addr, VA32, testdata);
	
	printf("VME:: Write to Address<0x%08x>  Data:<0x%08x>\n",address,data);
	
	return 0;
}


int o_READA32()
{

	UINT32 addr=0;
	UINT32 testdata=0;

	printf("ENTER YOUR A32 ADDRESS (0x..;BASE=0x0):");
	scanf("%x",&addr);

	/*ReadFrmVme32(addr,VA32,&testdata);*/
	Read_A32(addr,(int *)&testdata);
	printf("  0x%x   in   0x%x\n",testdata,addr);

	return 0;
}

int o_WRITEA32()
{

	UINT32 addr=0;
	UINT32 testdata=0;

	printf("ENTER YOUR A32 ADDRESS (0x..;BASE=0x0):");
	scanf("%x",&addr);

	printf("ENTER YOUR D32 DATA (0x..):");
	scanf("%x",&testdata);


	/*WriteToVme32(addr, VA32, testdata);*/
	Write_A32(addr,testdata);

	printf("A32 WRITE  0x%x  TO  0x%x  SUCCESS!!\n",testdata,addr);

	return 0;

}



void mywriteA32D32(unsigned Addr,unsigned val)
{
	WriteToVme32(Addr, VA32, val);
}
void myreadA32D32(unsigned Addr)
{
	unsigned  abc;
	ReadFrmVme32(Addr,VA32,&abc);
	printf("addr 0x%x = 0x%x\n",Addr,abc);
}


void mywriteA24D8(unsigned Addr,unsigned val)
{
	WriteToVme8(Addr, VA24, val);
}
void myreadA24D8(unsigned Addr)
{
	unsigned  char abc;
	ReadFrmVme8(Addr,VA24,&abc);
	printf("addr 0x%x = 0x%x\n",Addr,abc);
}


void mywriteA24D16(unsigned Addr,unsigned val)
{
	WriteToVme16(Addr, VA24, val);
}
void myreadA24D16(unsigned Addr)
{
	unsigned short abc;
	ReadFrmVme16(Addr,VA24,&abc);
	printf("addr 0x%x = 0x%x\n",Addr,abc);
}


void mywriteA24D32(unsigned Addr,unsigned val)
{
	WriteToVme32(Addr, VA24, val);
}

void myreadA24D32(unsigned Addr)
{
	unsigned  abc;
	ReadFrmVme32(Addr,VA24,&abc);
	printf("addr 0x%x = 0x%x\n",Addr,abc);
}


void mywriteCSRD8(unsigned Addr,unsigned val)
{
	WriteToVme8(Addr, VCSR, val);
}
void myreadCSRD8(unsigned Addr)
{
	unsigned  char abc;
	ReadFrmVme8(Addr,VCSR,&abc);
	printf("addr 0x%x = 0x%x\n",Addr,abc);
}

