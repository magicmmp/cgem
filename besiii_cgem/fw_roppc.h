/*fw_roppc.h*/
#ifndef __INCFWROPPCh
#define __INCFWROPPCh

void PPCbegin();
void PPCStart();
void Waiting();
void Dispatcher();
int 	ParseCmd(char * str);

/*some function define in kernel,but have no header define*/
#ifdef __cplusplus 
extern "C" {
#endif

extern STATUS sysUnivIntDisable
    (
    int univIntType /* interrupt type, sysLib */
    );
extern UCHAR sysNvRead
    (
    ULONG offset /* NVRAM offset to read the byte from,sysLib */
    );

extern void dmadisable(); /*define in kernel*/

#ifdef __cplusplus
}
#endif

#endif
