/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : 
 *
 * Purpose  : 
 *
\**************************************************************************/


#ifndef PIM_GROUPSET_H
#define PIM_GROUPSET_H

// group count
#define     GROUPNUM        11

//------------ group ----------------------------
#define	    SIMGRP					0	//	SIM
#define	    FAMILYGRP				1	//	family
#define	    FRIENDGRP				2	//	friend
#define	    WORKGRP				    3	//	colleague
#define	    BUSINESSGRP			    4	//	business
#define	    CUSTOMGRP1				5	//	freedom 1
#define	    CUSTOMGRP2				6	//	freedom 2
#define	    CUSTOMGRP3				7	//	freedowm 3
	
#define	    STRANGERGRP			    8	//	strange
#define	    ALLGRP					9	//	all 
#define	    FREQUENTGRP			    10  //	usual 

// field length of group
#define     GRPNAMELEN      10
#define     GRPICONLEN      50
#define     GRPRINGLEN      50
#define     GRPPENGLEN      50

//----------- group record ----------------------------		
typedef struct tagGROUPSET
{
	char GroupName[GRPNAMELEN+1];			// group name   11
	char IconName[GRPICONLEN+1];			// group icon   51
	char RingName[GRPRINGLEN+1];			// group ring   51
	char PengName[GRPPENGLEN+1];			// group animation 51
}GROUPSET;


#define     GRP_ALLFRQUNT	0x0001
#define     GRP_STRANGER	0x0002


#ifdef  __cplusplus
extern c {
#endif

int PB_ReadGroupSet(GROUPSET * Groups);
int PB_SaveGroupSet(GROUPSET * Groups, int index);
int PB_SaveGroupAll( GROUPSET * Groups);

#ifdef  __cplusplus
}
#endif

#endif