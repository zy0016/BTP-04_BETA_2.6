/***************************************************************************
*
*                      Pollex Mobile Platform
*
* Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
*                       All Rights Reserved 
*
* Module   : CaleSaveApi.c
*
* Purpose  : 
*
\**************************************************************************/

#include "CaleSaveApi.h"
#include "CaleAlarm.h"

static int CalNum;

int CALE_OpenApi(int CaleEntry)
{
    int Cal_OpenFile;
    char szOutPath[100];
    
    Cal_OpenFile = -1;
    CalNum = -1;

    memset(szOutPath, 0x0, 100);
    getcwd(szOutPath, 100);
    chdir(CALE_FILE_PATH);
    if(CaleEntry == CALE_MEETING)
    {
        Cal_OpenFile = open(CALE_MEETING_FILE, O_RDWR | O_CREAT, S_IRWXU);
        /*{
            struct stat buf;
            memset(&buf,0,sizeof(struct stat));
            stat(CALE_MEETING_FILE,&buf);
        }*/
    }
    if(CaleEntry == CALE_ANNI)
    {
        Cal_OpenFile = open(CALE_ANNIVERSARY_FILE, O_RDWR | O_CREAT, S_IRWXU);
    }
    if(CaleEntry == CALE_EVENT)
    {
        Cal_OpenFile = open(CALE_EVENT_FILE, O_RDWR | O_CREAT, S_IRWXU);
    }
    if(Cal_OpenFile < 0)
    {
        chdir(szOutPath);
        return CALE_FAIL;
    }
    return Cal_OpenFile;
}

BOOL CALE_CloseApi(int Cal_OpenFile)
{
    if(Cal_OpenFile > 0)
    {
        close(Cal_OpenFile);
        return TRUE;
    }
    return FALSE;
}

int CALE_ReadApi(int Cal_Open, int itemID, BYTE *pRecord, int size)
{
    int ID;
    char szOutPath[100];
    
    memset(szOutPath, 0x0, 100);
    getcwd(szOutPath, 100);
    chdir(CALE_FILE_PATH);

    if(itemID == -1)
    {
        CalNum ++;
        while(read(Cal_Open, pRecord, size) == size)
        {
            ID = CalNum;
            return ID;
        }
        CalNum = -1;
        return CALE_FAIL;
    }
    else
    {
        lseek(Cal_Open, (itemID)*size, SEEK_SET);
        if(read(Cal_Open, pRecord, size) == size)
        {
            return CALE_SUCCESS;
        }
        else
        {
            return CALE_FAIL;
        }
    }
}

int CALE_AddApi(int CaleEntry, int Cal_Open, BYTE *pRecord, int size)
{
    struct stat buf;
    int itemId;
    char szOutPath[100];
    
    memset(szOutPath, 0x0, 100);
    getcwd(szOutPath, 100);
    chdir(CALE_FILE_PATH);

    if(CaleEntry == CALE_MEETING)
    {
        stat(CALE_MEETING_FILE, &buf);
    }
    else if(CaleEntry == CALE_ANNI)
    {
        stat(CALE_ANNIVERSARY_FILE, &buf);
    }
    else if(CaleEntry == CALE_EVENT)
    {
        stat(CALE_EVENT_FILE, &buf);
    }
    itemId = buf.st_size / size;

    lseek(Cal_Open, 0, SEEK_END);
    if(write(Cal_Open, pRecord, size) == size)
    {
        chdir(szOutPath);
        return itemId;
    }
    else
    {
        chdir(szOutPath);
        return CALE_FAIL;
    }
}

BOOL CALE_ModifyApi(int Cal_Open, int itemID, BYTE *pRecord, int size)
{
    char szOutPath[100];
    
    memset(szOutPath, 0x0, 100);
    getcwd(szOutPath, 100);
    chdir(CALE_FILE_PATH);

    lseek(Cal_Open, itemID * size, SEEK_SET);
    if(write(Cal_Open, pRecord, size) == size)
    {
        chdir(szOutPath);
        return TRUE;
    }
    else
    {
        chdir(szOutPath);
        return FALSE;
    }
}

BOOL CALE_DelApi(int CaleEntry, int Cal_Open, int itemID, BYTE *pRecord, int size)
{
    int Cal_OpenFlag;
    int iSeek = 0;
    unsigned long lDelFlag;
    char szOutPath[100];
    
    memset(szOutPath, 0x0, 100);
    getcwd(szOutPath, 100);
    chdir(CALE_FILE_PATH);
    
    Cal_OpenFlag = -1;
    Cal_OpenFlag = open(CALE_DELETE_FLAG, O_RDWR | O_CREAT, S_IRWXU);
    if(Cal_OpenFlag < 0)
    {
		CALE_Print("\r\n[calendar]: CALE_DelApi Cal_OpenFlag < 0 !\r\n");
	//	CALE_Print("\r\n[calendar]: error code = %d\r\n", GetError());
        chdir(szOutPath);
        return FALSE;
    }

    lseek(Cal_Open, itemID * size, SEEK_SET);
    if(write(Cal_Open, pRecord, size) != size)
    {
		CALE_Print("\r\n[calendar]: CALE_DelApi write file failed!\r\n");
        close(Cal_OpenFlag);
        chdir(szOutPath);
        return FALSE;
    }

    if(CaleEntry == CALE_MEETING)
    {
        iSeek = 0;
    }
    if(CaleEntry == CALE_ANNI)
    {
        iSeek = 1;
    }
    if(CaleEntry == CALE_EVENT)
    {
        iSeek = 2;
    }

    lseek(Cal_OpenFlag, iSeek * sizeof(unsigned long), SEEK_SET);
    read(Cal_OpenFlag, &lDelFlag, sizeof(unsigned long));
    if(lDelFlag == 0)
    {
        close(Cal_OpenFlag);
        chdir(szOutPath);
        return TRUE;
    }
    else
    {
        lDelFlag = lDelFlag >> 1;
    }
    lseek(Cal_OpenFlag, iSeek * sizeof(unsigned long), SEEK_SET);
    write(Cal_OpenFlag, &lDelFlag, sizeof(unsigned long));
    
    close(Cal_OpenFlag);
    chdir(szOutPath);
    return TRUE;
}

BOOL CALE_CleanApi(void)
{
    int Cal_temp;
    int Cal_OpenFile;
    int Cal_OpenFlag;
    unsigned long lDelFlag;
    
    CaleMeetingNode pMeeting;
    CaleAnniNode pAnni;
    CaleEventNode pEvent;
    char szOutPath[100];
    
    memset(szOutPath, 0x0, 100);
    getcwd(szOutPath, 100);
    chdir(CALE_FILE_PATH);

    Cal_temp = -1;
    Cal_OpenFile = -1;
    Cal_OpenFlag = -1;
    lDelFlag = 0;

    //delete flag
    Cal_OpenFlag = open(CALE_DELETE_FLAG, O_RDWR | O_CREAT, S_IRWXU);
    if(Cal_OpenFlag < 0)
    {
        chdir(szOutPath);
        return FALSE;
    }
    lseek(Cal_OpenFlag, 0, SEEK_SET);
    read(Cal_OpenFlag, &lDelFlag, sizeof(unsigned long));
#ifdef _CALENDAR_LOCAL_TEST_
	if(lDelFlag < 0x0fffffff)
#else
    if(lDelFlag == 0)
#endif
    {
        //clean up meeting
        Cal_temp = open(CALE_TEMP_FILE, O_RDWR | O_CREAT, S_IRWXU);
        if(Cal_temp < 0)
        {
            close(Cal_OpenFlag);
            chdir(szOutPath);
            return FALSE;
        }
        Cal_OpenFile = open(CALE_MEETING_FILE, O_RDWR | O_CREAT, S_IRWXU);
        if(Cal_OpenFile < 0)
        {
            close(Cal_temp);
            close(Cal_OpenFlag);
            chdir(szOutPath);
            return FALSE;
        }
        memset(&pMeeting, 0x0, sizeof(CaleMeetingNode));
        while(read(Cal_OpenFile, &pMeeting, sizeof(CaleMeetingNode)) == sizeof(CaleMeetingNode))
        {
            if(pMeeting.LocalFlag == CALE_UNLOCAL)
            {
                memset(&pMeeting, 0x0, sizeof(CaleMeetingNode));
                continue;
            }
            if(write(Cal_temp, &pMeeting, sizeof(CaleMeetingNode)) != sizeof(CaleMeetingNode))
            {
                close(Cal_temp);
                close(Cal_OpenFile);
                close(Cal_OpenFlag);
                remove(CALE_TEMP_FILE);
                chdir(szOutPath);
                return FALSE;
            }
            memset(&pMeeting, 0x0, sizeof(CaleMeetingNode));
        }
        close(Cal_temp);
        close(Cal_OpenFile);
        Cal_temp = -1;
        Cal_OpenFile = -1;
        remove(CALE_MEETING_FILE);
        rename(CALE_TEMP_FILE, CALE_MEETING_FILE);
        
        lDelFlag = 0xFFFFFFFF;
        lseek(Cal_OpenFlag, 0, SEEK_SET);
        write(Cal_OpenFlag, &lDelFlag, sizeof(unsigned long));
    }
    
    lDelFlag = 0;
    lseek(Cal_OpenFlag, sizeof(unsigned long), SEEK_SET);
    read(Cal_OpenFlag, &lDelFlag, sizeof(unsigned long));
    if(lDelFlag == 0)
    {
        //clean up anniversary
        Cal_temp = open(CALE_TEMP_FILE, O_RDWR | O_CREAT, S_IRWXU);
        if(Cal_temp < 0)
        {
            close(Cal_OpenFlag);
            chdir(szOutPath);
            return FALSE;
        }
        Cal_OpenFile = open(CALE_ANNIVERSARY_FILE, O_RDWR | O_CREAT, S_IRWXU);
        if(Cal_OpenFile < 0)
        {
            close(Cal_temp);
            close(Cal_OpenFlag);
            chdir(szOutPath);
            return FALSE;
        }
        memset(&pAnni, 0x0, sizeof(CaleAnniNode));
        while(read(Cal_OpenFile, &pAnni, sizeof(CaleAnniNode)) == sizeof(CaleAnniNode))
        {
            if(pAnni.LocalFlag == CALE_UNLOCAL)
            {
                memset(&pAnni, 0x0, sizeof(CaleAnniNode));
                continue;
            }
            if(write(Cal_temp, &pAnni, sizeof(CaleAnniNode)) != sizeof(CaleAnniNode))
            {
                close(Cal_temp);
                close(Cal_OpenFile);
                close(Cal_OpenFlag);
                remove(CALE_TEMP_FILE);
                chdir(szOutPath);
                return FALSE;
            }
            memset(&pAnni, 0x0, sizeof(CaleAnniNode));
        }
        close(Cal_temp);
        close(Cal_OpenFile);
        Cal_temp = -1;
        Cal_OpenFile = -1;
        remove(CALE_ANNIVERSARY_FILE);
        rename(CALE_TEMP_FILE, CALE_ANNIVERSARY_FILE);
        
        lDelFlag = 0xFFFFFFFF;
        lseek(Cal_OpenFlag, sizeof(unsigned long), SEEK_SET);
        write(Cal_OpenFlag, &lDelFlag, sizeof(unsigned long));
    }

    lDelFlag = 0;
    lseek(Cal_OpenFlag, 2 * sizeof(unsigned long), SEEK_SET);
    read(Cal_OpenFlag, &lDelFlag, sizeof(unsigned long));
    if(lDelFlag == 0)
    {
        //clean up memo
        Cal_temp = open(CALE_TEMP_FILE, O_RDWR | O_CREAT, S_IRWXU);
        if(Cal_temp < 0)
        {
            close(Cal_OpenFlag);
            chdir(szOutPath);
            return FALSE;
        }
        Cal_OpenFile = open(CALE_EVENT_FILE, O_RDWR | O_CREAT, S_IRWXU);
        if(Cal_OpenFile < 0)
        {
            close(Cal_temp);
            close(Cal_OpenFlag);
            chdir(szOutPath);
            return FALSE;
        }

        memset(&pEvent, 0x0, sizeof(CaleEventNode));
        while(read(Cal_OpenFile, &pEvent, sizeof(CaleEventNode)) == sizeof(CaleEventNode))
        {
            if(pEvent.LocalFlag == CALE_UNLOCAL)
            {
                memset(&pEvent, 0x0, sizeof(CaleEventNode));
                continue;
            }
            if(write(Cal_temp, &pEvent, sizeof(CaleEventNode)) != sizeof(CaleEventNode))
            {
                close(Cal_temp);
                close(Cal_OpenFile);
                close(Cal_OpenFlag);
                remove(CALE_TEMP_FILE);
                chdir(szOutPath);
                return FALSE;
            }
            memset(&pEvent, 0x0, sizeof(CaleEventNode));
        }
        close(Cal_temp);
        close(Cal_OpenFile);
        Cal_temp = -1;
        Cal_OpenFile = -1;
        remove(CALE_EVENT_FILE);
        rename(CALE_TEMP_FILE, CALE_EVENT_FILE);

        lDelFlag = 0xFFFFFFFF;
        lseek(Cal_OpenFlag, 2 * sizeof(unsigned long), SEEK_SET);
        write(Cal_OpenFlag, &lDelFlag, sizeof(unsigned long));
    }

    close(Cal_OpenFlag);
    chdir(szOutPath);
    
    return TRUE;
}

int CALE_ExternAddApi(int CaleEntry, BYTE *pRecord)
{
    int Cal_OpenFile;
    char szOutPath[100];
    int Nodesize;
    int itemId;
    struct stat buf;
    
    Cal_OpenFile = -1;

    memset(szOutPath, 0x0, 100);
    getcwd(szOutPath, 100);
    chdir(CALE_FILE_PATH);

    if(CaleEntry == CALE_MEETING)
    {
        Cal_OpenFile = open(CALE_MEETING_FILE, O_RDWR | O_CREAT, S_IRWXU);
        Nodesize = sizeof(CaleMeetingNode);
        stat(CALE_MEETING_FILE, &buf);
    }
    if(CaleEntry == CALE_ANNI)
    {
        Cal_OpenFile = open(CALE_ANNIVERSARY_FILE, O_RDWR | O_CREAT, S_IRWXU);
        Nodesize = sizeof(CaleAnniNode);
        stat(CALE_ANNIVERSARY_FILE, &buf);
    }
    if(CaleEntry == CALE_EVENT)
    {
        Cal_OpenFile = open(CALE_EVENT_FILE, O_RDWR | O_CREAT, S_IRWXU);
        Nodesize = sizeof(CaleEventNode);
        stat(CALE_EVENT_FILE, &buf);
    }

    if(Cal_OpenFile < 0)
    {
        chdir(szOutPath);
        return -1;
    }

    itemId = buf.st_size / Nodesize;

	CALE_Print("\r\n[calendar]: ExternAddApi, CalOpenFile = 0x%08x, size = %d\r\n", Cal_OpenFile, buf.st_size);
	CALE_Print("\r\n[calendar]: ExternAddApi, itemId = %d\r\n", itemId);
    
    lseek(Cal_OpenFile, 0, SEEK_END);
    if(write(Cal_OpenFile, pRecord, Nodesize) != Nodesize)
    {
		CALE_Print("\r\n[calendar]: ExternAddApi, write failed!!!!!!!\r\n");
        itemId = -1;
    }

    close(Cal_OpenFile);
    chdir(szOutPath);
    
	//////////////////////////////////////////////////////////////////////////
	if(itemId >= 0)
	{
		if(CaleEntry == CALE_MEETING)
		{
			if(((CaleMeetingNode*)pRecord)->MeetAlarmFlag)
				CALE_NewMeet((CaleMeetingNode*)pRecord, itemId, ALMF_REPLACE);
		}
		else if(CaleEntry == CALE_ANNI)
		{
			if(((CaleAnniNode*)pRecord)->AnniAlarmFlag)
				CALE_NewAnni((CaleAnniNode*)pRecord, itemId, ALMF_REPLACE);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	
    return itemId;
}

BOOL CALE_ExternDelApi(int CaleEntry, int CaleID)
{
    int Cal_OpenFile;
    CaleMeetingNode TmpMeetNode;
    CaleAnniNode TmpAnniNode;
    CaleEventNode TmpEventNode;

    Cal_OpenFile = CALE_OpenApi(CaleEntry);
    if(Cal_OpenFile == CALE_FAIL)
    {
        return FALSE;
    }

    if(CaleEntry == CALE_MEETING)
    {
        memset(&TmpMeetNode, 0x0, sizeof(CaleMeetingNode));
        CALE_ReadApi(Cal_OpenFile, CaleID, (BYTE *)&TmpMeetNode, sizeof(CaleMeetingNode));
		if(TmpMeetNode.MeetAlarmFlag)
			CALE_KillAlarm(CALE_MEETING, CaleID);
        TmpMeetNode.LocalFlag = CALE_UNLOCAL;
        CALE_DelApi(CaleEntry, Cal_OpenFile, CaleID, (BYTE *)&TmpMeetNode, sizeof(CaleMeetingNode));
    }
    else if(CaleEntry == CALE_ANNI)
    {
        memset(&TmpAnniNode, 0x0, sizeof(CaleAnniNode));
        CALE_ReadApi(Cal_OpenFile, CaleID, (BYTE *)&TmpAnniNode, sizeof(CaleAnniNode));
		if(TmpAnniNode.AnniAlarmFlag)
			CALE_KillAlarm(CALE_ANNI, CaleID);
        TmpAnniNode.LocalFlag = CALE_UNLOCAL;
        CALE_DelApi(CaleEntry, Cal_OpenFile, CaleID, (BYTE *)&TmpAnniNode, sizeof(CaleAnniNode));
    }
    else if(CaleEntry == CALE_EVENT)
    {
        memset(&TmpEventNode, 0x0, sizeof(CaleEventNode));
        CALE_ReadApi(Cal_OpenFile, CaleID, (BYTE *)&TmpEventNode, sizeof(CaleEventNode));
        TmpEventNode.LocalFlag = CALE_UNLOCAL;
        CALE_DelApi(CaleEntry, Cal_OpenFile, CaleID, (BYTE *)&TmpEventNode, sizeof(CaleEventNode));
    }
    CALE_CloseApi(Cal_OpenFile);

    return TRUE;
}

BOOL CALE_ExternModApi(int CaleEntry, int CaleID, BYTE *pRecord, int size)
{
    int Cal_OpenFile;
    
    Cal_OpenFile = CALE_OpenApi(CaleEntry);
    if(Cal_OpenFile == CALE_FAIL)
    {
        return FALSE;
    }
    
    CALE_ModifyApi(Cal_OpenFile, CaleID, pRecord, size);

    CALE_CloseApi(Cal_OpenFile);

	//////////////////////////////////////////////////////////////////////////
	if(CALE_MEETING == CaleEntry)
	{
		if(((CaleMeetingNode*)pRecord)->MeetAlarmFlag)
			CALE_NewMeet((CaleMeetingNode*)pRecord, CaleID, ALMF_REPLACE);
	}
	else if(CALE_ANNI == CaleEntry)
	{
		if(((CaleAnniNode*)pRecord)->AnniAlarmFlag)
			CALE_NewAnni((CaleAnniNode*)pRecord, CaleID, ALMF_REPLACE);
	}
    //////////////////////////////////////////////////////////////////////////
    
    return TRUE;
}

