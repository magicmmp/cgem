#include "stdio.h"
#include "vmerw.h"


int Read_A32(int address,int * data);
int Write_A32(int address,int data);

int o_READA32();
int o_WRITEA32();

void mywriteA24D8(unsigned Addr,unsigned val);
void myreadA24D8(unsigned Addr);
void mywriteA24D16(unsigned Addr,unsigned val);
void myreadA24D16(unsigned Addr);
void mywriteA24D32(unsigned Addr,unsigned val);
void myreadA24D32(unsigned Addr);
void mywriteCSRD8(unsigned Addr,unsigned val);
void myreadCSRD8(unsigned Addr);