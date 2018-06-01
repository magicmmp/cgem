#ifndef __INCFWACTIONSh
#define __INCFWACTIONSh

void SendAck(char * p_cAck) ;
/*Actions*/
int 	fw_InitializedLOAD (int nArg1) ;
int 	fw_LoadedPRCF (int nArg1) ;
int 	fw_PreconfigedCONF (int nArg1) ;
int 	fw_ConfigedSYN1 (int nArg1) ;
int 	fw_Synchronized1SYN2 (int nArg1) ;
int 	fw_Synchronized2SYN3 (int nArg1) ;
int 	fw_Synchronized3SYN4 (int nArg1) ;
int 	fw_Synchronized4PREP (int nArg1) ;
int 	fw_ReadySATR  (int nArg1);
int 	fw_RunningSPTR  (int nArg1);/*pointer pointing to the function that exectues subsystem-dependent RunningSPTR*/
int 	fw_ReadySTOP  (int nArg1);	/*pointer pointing to the function that exectues subsystem-dependent ReadySTOP*/
int 	fw_ConfigedUNCF (int nArg1);
int 	fw_PreconfigedUNPC (int nArg1) ;
int 	fw_LoadedUNLD (int nArg1);
int	fw_AnyEXIT(int nArg1) ;
int	fw_SelfTrans(int nArg1);
int	fw_AnyImpo(int nArg1);
#endif
