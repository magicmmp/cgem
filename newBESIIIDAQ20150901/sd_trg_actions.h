/*****sd_trg_actions.h*****/
#ifndef __INCSDTRGACTIONh
#define __INCSDTRGACTIONh

int	TRGInitializedLOAD 		(int nArg) ;
int  	TRGLoadedPRCF 			(int nArg) ;
int  	TRGPreconfigedCONF 	(int nArg) ;
int  	TRGConfigedSYN1 		(int nArg) ;
int  	TRGSynchronized1SYN2 	(int nArg) ;
int  	TRGSynchronized2SYN3 	(int nArg) ;
int  	TRGSynchronized3SYN4 	(int nArg) ;
int  	TRGSynchronized4PREP 	(int nArg) ;
int  	TRGReadySATR  			(int nArg) ;
int  	TRGRunningSPTR  		(int nArg) ;
int  	TRGReadySTOP  			(int nArg) ;	
int  	TRGConfigedUNCF 		(int nArg) ;
int  	TRGPreconfigedUNPC 	(int nArg) ;
int  	TRGLoadedUNLD 			(int nArg) ;
int  	TRGAnyEXIT 			(int nArg) ;	

#endif

