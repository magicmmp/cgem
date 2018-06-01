#ifndef __INCSDMDCACTIONSh
#define __INCSDMDCACTIONSh

int	MDCInitializedLOAD 		(int nArg) ;
int  	MDCLoadedPRCF 		(int nArg) ;
int  	MDCPreconfigedCONF 	(int nArg) ;
int  	MDCConfigedSYN1 		(int nArg) ;
int  	MDCSynchronized1SYN2 (int nArg) ;
int  	MDCSynchronized2SYN3 (int nArg) ;
int  	MDCSynchronized3SYN4 (int nArg) ;
int  	MDCSynchronized4PREP (int nArg) ;
int  	MDCReadySATR  			(int nArg);
int  	MDCRunningSPTR  		(int nArg);
int  	MDCReadySTOP  			(int nArg);	
int  	MDCConfigedUNCF 		(int nArg);
int  	MDCPreconfigedUNPC 	(int nArg) ;
int  	MDCLoadedUNLD 		(int nArg);
int  	MDCAnyEXIT 		(int nArg);	

void MDCPrintInfo(); //leigk060720
#endif
