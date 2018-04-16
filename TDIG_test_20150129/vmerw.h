/* Vmerw head file */

#ifndef VMERW_H
#define VMERW_H

#include "vxWorks.h"

/*#define VA16 0xffff0000
*/
#define VA24 0xfa000000
#define VA32 0x20000000
#define VCSR 0x30000000

#ifdef __cplusplus 
	/* 8-bit read from VME */
	/*RETURNS: 0 if succeed or -1 if failed*/
	extern "C"  int ReadFrmVme8(UINT32 nVmeBusAdrs, /*VME BUS address*/
					UINT32 nAdrsWidth,				/*Address type, only VA16, VA24, VA32 supported*/
					UINT8 *pData) ;

	/* 16-bit read from VME*/
	extern "C"  int ReadFrmVme16(UINT32 nVmeBusAdrs, 
				
	UINT32 nAdrsWidth, 
					UINT16 *pData) ;

	/* 32-bit read from VME*/
	extern "C"  int ReadFrmVme32(UINT32 nVmeBusAdrs, 
					UINT32 nAdrsWidth, 
					UINT32 *pData) ;

	/* 8-bit write to VME*/
	extern "C"	int WriteToVme8(UINT32 nVmeBusAdrs, 
				UINT32 nAdrsWidth, 
				UINT8 nData
				) ;

	/* 16-bit write to VME*/
	extern "C"	int WriteToVme16(UINT32 nVmeBusAdrs, 
				UINT32 nAdrsWidth, 
				UINT16 nData 
				) ;

	/* 32-bit write to VME*/
	extern "C"	int WriteToVme32(UINT32 nVmeBusAdrs, 
				UINT32 nAdrsWidth, 
				UINT32 nData 
				) ;

	/* 32-bit MCST write */
	extern "C"	int MCSTwrite(UINT32 nVmeBusAdrs,
						int  nDataWidth,
						UINT32 nData
						);



#else 
	/* 8-bit read from VME*/
	int ReadFrmVme8(UINT32 nVmeBusAdrs, 
				UINT32 nAdrsWidth, 
				UINT8 *pData) ;

	/* 16-bit read from VME*/
	int ReadFrmVme16(UINT32 nVmeBusAdrs, 
				UINT32 nAdrsWidth, 
				UINT16 *pData) ;

	/* 32-bit read from VME*/
	int ReadFrmVme32(UINT32 nVmeBusAdrs, 
				UINT32 nAdrsWidth, 
				UINT32 *pData) ;

	/* 8-bit write to VME*/
	int WriteToVme8(UINT32 nVmeBusAdrs, 
			UINT32 nAdrsWidth, 
			UINT8 nData
			) ;

	/* 16-bit write to VME*/
	int WriteToVme16(UINT32 nVmeBusAdrs, 
			UINT32 nAdrsWidth, 
			UINT16 nData
			) ;

	/* 32-bit write to VME*/
	int WriteToVme32(UINT32 nVmeBusAdrs, 
			UINT32 nAdrsWidth, 
			UINT32 nData 
			) ;

	/* 32-bit MCST write */
	int MCSTwrite(UINT32 nVmeBusAdrs,
			int  nDataWidth,
			UINT32 nData);

#endif   /*end of #ifdef __cplusplus */

#endif  /*end of #ifndef VMERW_H*/
