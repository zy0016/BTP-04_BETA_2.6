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

/*---------- Dependencies --------------------------------------*/
#include "string.h"
#include "stdlib.h"
#include "str_public.h"
#include "str_public_chn.h"
#include "str_public_eng.h"
#include "str_plx.h"
#include "str_plx_chn.h"
#include "str_plx_eng.h"

//////////////////////////////2003-11-12////////////////////////////////////////////
extern	BOOL	SetSavedLanguage(int lang);//
extern	int		GetLanguage(void);
/////////////////////////////2003-11-12/////////////////////////////////////////////

//#include "init_global_def.h" /* set it to flash */

/*---------- Global Definitions and Declarations ---------------*/
static int gCurrentLanguage = STR_LANG_SIMPCHN; /* current language */
/* store the all of language tab */
static  unsigned char **LanguageTab[] ={
    (unsigned char **)strEng,
	(unsigned char **)strChn,
   //strTra,
};

static  unsigned char **PLXLanguage[] ={
    (unsigned char **)hp_strEng,
    (unsigned char **)hp_strChn,
};
 
/*---------- Constant / Macro Definitions ----------------------*/
/* Soft key button message ID definition */

/*---------- Type Declarations ---------------------------------*/

/*---------- Variable Declarations -----------------------------*/
/*---------- function Declarations -----------------------------*/
/*------------------END-----------------------------------------*/

/**************************************************************
 FUNCTION   :  GetSring                                     
---------------------------------------------------------------
 DESCRIPTION:  return the string by the strID with current 
               language.
---------------------------------------------------------------
 INPUT		:strID  ----- string ID value
             
---------------------------------------------------------------
 RETURN		: *str   ----- str point for ouput
              success : return the string;
			  fail    : return NULL.
---------------------------------------------------------------
 IMPORTANT NOTES:    
             called by  other modul .
***************************************************************/
unsigned char* GetString (WINOS_STRING strID)
{

	int iTemp;
	
	iTemp = STR_PLX_MAX;

	if (!(((strID < STR_STRING_MAX)&&(strID > 0))||
		((strID < STR_PLX_MAX)&&(strID >= 5000))))
	{
		strID = 0; /* 0 is NULL string*/
	}

    if( strID >= 5000 )
	{
       return *((unsigned char **)PLXLanguage[gCurrentLanguage]+strID - 5000);
	}
	else
	{
	   return *((unsigned char **)LanguageTab[gCurrentLanguage]+strID);
	}
}
/**************************************************************
 FUNCTION   : SetCurrentLanguage                                      
---------------------------------------------------------------
 DESCRIPTION: Set the current language  table        
---------------------------------------------------------------
 INPUT		: lang	----- languge setting vaule                   
---------------------------------------------------------------
 RETURN		: int ----- 0: fail 1: successful                                               
---------------------------------------------------------------
 IMPORTANT NOTES:     
            this function will be use on the user select the 
			language list value or the system initializtion.
			it will save language value inot to flash. 

***************************************************************/
int SetCurrentLanguage(int lang)
{
	int ret = 0;
    if ((lang < STR_LANG_MAX)||(lang >= 0))
    { 
	  gCurrentLanguage = lang;
	  //SetSavedLanguage(lang);//2003-11-12
	  //INIT_SetCfgEle(ID_LANGUAGE, &lang); /*save it into flash*/
	  ret = 1;
	}
	return ret;
}
/**************************************************************
 FUNCTION   : GetCurrentLanguage                                      
---------------------------------------------------------------
 DESCRIPTION: get the current language  table        
---------------------------------------------------------------
 INPUT		: lang	----- languge setting vaule                   
---------------------------------------------------------------
 RETURN		: language index ID                                               
---------------------------------------------------------------
 IMPORTANT NOTES:     
***************************************************************/
int GetCurrentLanguage()
{
	return gCurrentLanguage;
	//return (GetLanguage());//2003-11-12
}
/**************************************************************
 FUNCTION   :  GetSpecificString                                     
---------------------------------------------------------------
 DESCRIPTION:  return the string by the strID with specific 
               language.
---------------------------------------------------------------
 INPUT		:strID  ----- string ID value
            :LanID  ----- language ID value
             
---------------------------------------------------------------
 RETURN		: *str   ----- str point for ouput
              success : return the string;
			  fail    : return NULL.
---------------------------------------------------------------
 IMPORTANT NOTES:    
             called by  other modul .
***************************************************************/
unsigned char* GetSpecificString (WINOS_STRING strID,WINOS_LANGUAGE LanID)
{
    if ((LanID > STR_LANG_MAX)||(LanID < 0))
	{
	  LanID = 0; /* 0 is NULL string*/
    }

	if (!(((strID < STR_STRING_MAX)&&(strID > 0))||
		((strID < STR_PLX_MAX)&&(strID >= 5000))))
	{
		strID = 0; /* 0 is NULL string*/
	}
  

	if( strID >= 5000 )
	{
       return *((unsigned char **)PLXLanguage[LanID]+strID - 5000);
	}
	else
	{
		return *((unsigned char **)LanguageTab[LanID]+strID);
	}
}
/***************************end********************************/


WINOS_STRING GetStringID(unsigned char *str)
{
	WINOS_STRING strID;
	UINT		 uLangNum;
	for(uLangNum = 0; uLangNum < STR_LANG_MAX; uLangNum ++)
	{
		for( strID = 0; strID < STR_STRING_MAX; strID ++)
		{
			if(strcmp(*((unsigned char **)LanguageTab[uLangNum]+strID),str) == 0)
				return strID;
		}
	}
	return -1;
}

#define BACKGROUNDIMAGECOUNT    5
 
unsigned char *BkImage[] = 
{
    "desk1.bmp",
    "desk2.bmp",
    "desk3.bmp",
    "desk4.bmp",
    "desk5.bmp"
};
 
char **BackgroundImage = BkImage;
 
char** GetBackgroundImage(int* ImageNum)
{
 *ImageNum = BACKGROUNDIMAGECOUNT;
 return BackgroundImage;
}
