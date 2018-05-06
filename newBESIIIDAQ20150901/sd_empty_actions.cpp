/*sd_empty.cpp, test an empty system*/

#include "vxWorks.h"
#include "stdio.h"

#include "fw_roppc.h"
#include "fw_dcgvas.h"

#include "sd_empty_actions.h"

int	EMPTYInitializedLOAD 		(int nArg) 
{
	printf("EMPTYInitializedLOAD OK\n") ;
	return 0;
}
int  	EMPTYLoadedPRCF 			(int nArg) 
{
	printf("EMPTYLoadedPRCF OK\n") ;
	return 0;
}
int  	EMPTYPreconfigedCONF 	(int nArg) 
{
	printf("EMPTYPreconfiged OK\n") ;
	return 0;
}
int  	EMPTYConfigedSYN1 		(int nArg) 
{
	printf("EMPTYConfigedSYN1 OK\n") ;
	return 0;
}
int  	EMPTYSynchronized1SYN2 	(int nArg) 
{
	printf("EMPTYSynchronized1SYN2 OK\n") ;
	return 0;
}
int  	EMPTYSynchronized2SYN3 	(int nArg) 
{
	printf("EMPTYSynchronized2SYN3 OK\n") ;
	return 0;
}
int  	EMPTYSynchronized3SYN4 	(int nArg) 
{
	printf("EMPTYSynchronized3SYN4 OK\n") ;
	return 0;
}
int  	EMPTYSynchronized4PREP 	(int nArg) 
{
	printf("EMPTYSynchronized4PREP OK\n") ;
	return 0;
}
int  	EMPTYReadySATR  			(int nArg)
{
	printf("EMPTYReadySATR OK\n") ;
	return 0;
}
int  	EMPTYRunningSPTR  		(int nArg)
{
	printf("EMPTYRunningSPTR OK\n") ;
	return 0;
}
int  	EMPTYReadySTOP  			(int nArg)
{
	printf("EMPTYReadySTOP OK\n") ;
	return 0;
}
int  	EMPTYConfigedUNCF 		(int nArg)
{
	printf("EMPTYConfigedUNCF OK\n") ;
	return 0;
}
int  	EMPTYPreconfigedUNPC 	(int nArg) 
{
	printf("EMPTYPreconfigedUNPC OK\n") ;
	return 0;
}
int  	EMPTYLoadedUNLD 			(int nArg)
{
	printf("EMPTYLoadedUNLD OK\n") ;
	return 0;
}
int  	EMPTYAnyEXIT 			(int nArg)
{
	printf("EMPTYAnyEXIT OK\n") ;
	return 0;
}
