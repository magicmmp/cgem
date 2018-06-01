/***************************************************************************
                          FragHeaders.h  -  description
                             -------------------
    begin                :  5th Nov 13:13:32 CST 2004
    copyright            : (C) 2004 by renzy
    email                : renzy@mail.ihep.ac.cn
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
/*renzy: mostly refers to ROSEventFragment/.../ROS(D,B)Fragment.h and EventFragment.h*/

#ifndef R_FRAGHEADERS_H
#define R_FRAGHEADERS_H


typedef struct {
         unsigned int startOfHeaderMarker;
         unsigned int totalFragmentsize;
         unsigned int headerSize;
         unsigned int formatVersionNumber;
         unsigned int sourceIdentifier;
         unsigned int runNumber;
}  GenericHeader;








typedef struct {
         unsigned int level1Id;
         unsigned int bunchCrossingId;
} ROSHeader;/*renzy:ROS specifi Header*/








typedef struct {
	unsigned int startOfHeaderMarker;
	unsigned int headerSize;
	unsigned int formatVersionNumber;
	unsigned int sourceIdentifier;
	unsigned int runNumber;
	unsigned int level1Id;
	unsigned int bunchCrossingId;
	unsigned int level1TriggerType;
	unsigned int detectorEventType;	//bit 0 : 0 -no calib;1-calib;
									//bit 1:0- good event;1-abnormal event;
} RODHeader;

typedef struct {
	unsigned int numberOfStatusElements;
	unsigned int numberOfDataElements;
	unsigned int statusBlockPosition;
} RODTrailer;




/*static const int s_nStatusElements = 2;
 Fix the number of status elements.  Should this be variable?
static const int s_nOffsetElements = 1;*/


typedef struct {
         unsigned int level1Id;
         unsigned int bunchCrossingId;
         unsigned int level1TriggerType;
         unsigned int detectorEventType;
} ROBSpecificHeader;


typedef struct {
         GenericHeader generic;
         unsigned int numberOfStatusElements;
         /*unsigned int statusElement[s_nStatusElements];*/
	 unsigned int statusElement [2];
         unsigned int numberOfOffsetElements;
         /*unsigned int offsetElement[s_nOffsetElements];*/
	 unsigned int offsetElement[1];
         unsigned int numberOfFragmentSpecificElements;
         ROBSpecificHeader specific;
} ROBHeader;

static const unsigned int s_formatVersionNumber = 0x03000000;
static const unsigned int s_rodMarker = 0xee1234ee;
static const unsigned int s_robMarker = 0xdd1234dd;
static const unsigned int s_rosMarker = 0xcc1234cc;
#endif
