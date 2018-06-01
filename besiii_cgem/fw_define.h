/*-----------------------------
 fw_roppc.h
 based zhuht edit version
 begin                :  3th Dec 16:01:32 CST 2006
 copyright            : (C) 2004 by leigk
 email                : leigk@mail.ihep.ac.cn
 description: the information to config the VME crate
-----------------------------*/
#ifndef __INCFWDEFINEh
#define __INCFWDEFINEh

#include "vxWorks.h"
#include "stdio.h"
#include "sockLib.h"
#include "sys/socket.h"

#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netinet/tcp.h>
#include <netinet/in_pcb.h>
#include <netinet/ip_var.h>
#include <netinet/tcp_var.h>
#include <net/unixLib.h>

#include "inetLib.h"
#include "ioLib.h"
#include "wdLib.h"

/* Macros matching paticular electronic modules */


#define DEBUG_MODE

#define DEBUG_OUTPUT(x)  printf x



/*ID of each sub-system*/
#define	SYSID_EMPTY 		0
#define	SYSID_MDC			1
#define	SYSID_TOF			2
#define	SYSID_EMC			3
#define	SYSID_MUC			4
#define	SYSID_TRG			5
#define	SYSID_ZDD			6
#define	SYSID_ETF			7

#define	MAX_SYS_AMOUNT 	8
#define 	MAX_SLOT_AMOUNT_PLUS_ONE	22 /*VME crate have 21slots*/
#define 	MAX_CHNL_AMOUNT 				32 /*per fee module most have 32 channle*/

/*PORT of address*/
#define LOCAL_SERV_PORT 9230 /*port of the socket carring commands and acknowledgment*/

/*Priorities*/
#define PRI_WAITING 145
#define PRI_DISPATCHER 145 
#define PRI_CURNSTATE 155

#define PRI_REPORTSTATUS (PRI_CURNSTATE -5)
#define PRI_CBLTTRANS (PRI_CURNSTATE + 5)
#define PRI_DATAPACK (PRI_CURNSTATE + 15)
#define PRI_NETTRANS (PRI_CURNSTATE + 10)
#define PRI_TAKEDATA PRI_CBLTTRANS

#define TASK_STACK_SIZE 20000

/*command & states*/
#define MAX_CMD_AMOUNT 15
#define MAX_STATES_AMOUNT 11
/*command ID*/
#define CMID_LOAD 0		
#define CMID_PRCF  1		
#define CMID_CONF 2		
#define CMID_SYN1 3		
#define CMID_SYN2 4		
#define CMID_SYN3 5		
#define CMID_SYN4 6		
#define CMID_PREP 7		
#define CMID_SATR 8		
#define CMID_SPTR 9		
#define CMID_STOP 10		
#define CMID_UNCF 11		
#define CMID_UNPC 12		
#define CMID_UNLD 13		
#define CMID_EXIT  14		
#define CMID_UNKN 15		
/*state ID*/
#define STID_Waiting 		0
#define STID_Initialized 		1
#define STID_Loaded 			2
#define STID_PreConfiged 	3
#define STID_Configed 		4
#define STID_Synchronized1	5
#define STID_Synchronized2 	6
#define STID_Synchronized3 	7
#define STID_Synchronized4 	8
#define STID_Ready 			9
#define STID_Running 		10

/*ack*/
#define SUCC "SUCC"
#define FAIL  "FAIL"
#define IMPO "IMPO"
#define CMD_BYTES 4
#define ACK_BYTES 4

//#define MAXNUM_MSGS 2000          // leigk 8000
#define MAXNUM_MSGS 2000
#define DMARNG_SIZE (80*4096)*16 //5MBytes
#define NETRNG_SIZE (80*4096)*16

/*running tasks control*/
#define RUNNING_NOW 2

#define CBLT_STOP 4
#define DPAC_STOP 5
#define NET_STOP 6

#define FW_MAX_RETRY 1000
#define TIMEOUT_TICKS 1

#define LAST_EVENT_RODMARKER 0xee1235ee

/*running mode*/
#define NOMALMODE		0
#define BASELINE		1	
#define CALIBRATION		2
#define PEDMODE			3
#define WAVEMODE		4
#define MONITOR			5


#define VA16 0xfbff0000
#define VA24 0xfa000000
#define VA32 0x20000000
#define VCSR 0x30000000

#define VALUE_OF_A16D16(x) (*(UINT16*)(VA16 + (x)))
#define VALUE_OF_A24D16(x) (*(UINT16*)(VA24 + (x)))
#define VALUE_OF_A24D32(x) (*(UINT32*)(VA24 + (x)))


#endif
