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
#include "smsglobal.h"


BOOL CanPaste(void)
{
     return FALSE;
}

BOOL CanInsertTemplate(void)
{
    return TRUE;
}
/*********************************************************************\
* Function	   IsFlashEnough
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
BOOL IsFlashEnough(void)
{
    int nRet;

    if(SMS_GetMode() == TRUE)
        return FALSE;
    else
    {        
        nRet = IsFlashAvailable(MAX_TXT_SIZE/1024);
        
        if(nRet == SPACE_AVAILABLE || nRet == SPACE_NEEDCLEAR)
            return TRUE;
        else
            return FALSE;
    }
}

/*********************************************************************\
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
void SMS_PleaseToWait(void)
{
    PLXTipsWin(NULL,NULL,0,"Please waiting...we \r\n will correct this \r\n problem in \r\nAlpha1.0","Prompt",
        Notify_Info,IDS_OK,NULL,WAITTIMEOUT);
}


BOOL DeleteAllSMS(void);
BOOL IsBeFirst(void);
/*********************************************************************\
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
void MaybeDelete(void)
{
    if(IsBeFirst() == TRUE)
    {
        if(DeleteAllSMS() == TRUE)
        {            
            char szOldPath[PATH_MAXLEN];
            int f;
            int first = 0;
            
            szOldPath[0] = 0;
            
            getcwd(szOldPath,PATH_MAXLEN);  
            
            chdir(PATH_DIR_SMS);  
            
            f = open("smscounter2.tmp",O_RDWR);
            
            if(f == -1)
            {
                chdir(szOldPath);  
                
                return;
            }
            
            write(f,&first,sizeof(int));
            
            close(f);
            
            chdir(szOldPath);  
        }
    }
}
/*********************************************************************\
* Function	   DeleteAllSMS
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
BOOL DeleteAllSMS(void)
{
    struct dirent* pData;
    DIR* pDir;
    char szOldPath[PATH_MAXLEN];

    szOldPath[0] = 0;
    
    getcwd(szOldPath,PATH_MAXLEN);  
    chdir(PATH_DIR_SMS);  

    remove(SMS_FILENAME_MESTOREINFO);

    pDir = opendir(PATH_DIR_SMS);
    if(pDir == NULL)
    {
        chdir(szOldPath);
        return FALSE;
    }

    while( (pData = readdir(pDir)) != NULL )
    {
        if(strstr(pData->d_name,".sms") != NULL)
            remove(pData->d_name);
    }
    
    closedir(pDir);
    pDir = NULL;
    chdir(szOldPath);
    return TRUE;
}
/*********************************************************************\
* Function	   IsBeFirst
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
BOOL IsBeFirst(void)
{
    char szOldPath[PATH_MAXLEN];
    int f;
    int first = -1;

    szOldPath[0] = 0;
    
    getcwd(szOldPath,PATH_MAXLEN);  

    chdir(PATH_DIR_SMS);  

    f = open("smscounter2.tmp",O_RDONLY);

    if(f == -1)
    {
        f = open("smscounter2.tmp",O_RDWR | O_CREAT,S_IRWXU);

        if(f == -1)
        {
            chdir(szOldPath);  
            
            return FALSE;
        }

        first = 1;

        write(f,&first,sizeof(int));

        close(f);

        chdir(szOldPath);  
        
        return TRUE;
    }

    read(f,&first,sizeof(int));

    close(f);

    chdir(szOldPath);  

    if(first == 0)
        return FALSE;
    else
        return TRUE;    
}
