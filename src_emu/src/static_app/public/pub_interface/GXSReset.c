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

//#include "fapi.h"
#include "stdlib.h"
#include "sys/types.h"
#include "sys/statfs.h"
#include "sys/stat.h"
#include "sys/ioctl.h"
#include "fcntl.h"
#include "unistd.h" 
#include "dirent.h"

#include "stdio.h"
#include "window.h"
#include "plx_pdaex.h"
#include "setting.h"
#include "string.h"
#include "pubapp.h"
#include "info.h"
//extern void ImeDefaultSetup();



#define	PHONEFLASH	"/mnt/flash"
#define	MMC	"/mnt/fat16"

#ifndef _EMULATE_
#define	ERRSPACE	1024
#define	WARNSPACE	1536
#else
#define	ERRSPACE	100
#define	WARNSPACE	200
#endif

extern	BOOL	DeleteDirectory(PCSTR	pPath, BOOL b);
extern BOOL		MMC_CheckCardStatus();
extern	BOOL	IsPicture(PSTR	pFileName, int *nItemData);

int	GetAvailSpace(PCSTR	pDevPath);


int	GetAvailFlashSpace()
{
	return	GetAvailSpace(PHONEFLASH);
}
int	GetAvailMMCSpace()
{
	if (MMC_CheckCardStatus()) {
		return	GetAvailSpace(MMC);
	}
	else
		return 0;
	
}
/*
 *	return kB
 */
int	GetAvailSpace(PCSTR	pDevPath)
{
	struct	statfs	spaceinfo;
	statfs(pDevPath, &spaceinfo);
	return spaceinfo.f_bavail* spaceinfo.f_bsize/1024;
}
int	GetPhoneImageSpace(PSTR	pPath)
{
	
	struct dirent *dirinfo = NULL;
	struct stat	filestat;
	DIR  *dirtemp = NULL;
	int		i=0;
	int		dFileType;
	PSTR	strFullName;
	int		nTotal = 0;
//	int		tmp;
	dirtemp = opendir(pPath);
	if (!dirtemp) {
		return 0;
	}
	dirinfo = readdir(dirtemp);
	while (dirinfo&&dirinfo->d_name[0]) {
		strFullName = malloc(strlen(pPath)+strlen(dirinfo->d_name)+2);
		strcpy ( strFullName, pPath );
		if (pPath[strlen(pPath)-1] != '/') {
			strcat(strFullName,"/");
		}
		strcat ( strFullName, dirinfo->d_name);
		stat(strFullName,&filestat);
		if (((!S_ISLNK(filestat.st_mode))&&(S_ISDIR(filestat.st_mode))&&(strcmp(dirinfo->d_name,".")!=0)&&(strcmp(dirinfo->d_name,"..")!=0))) 
		{
			//unlink(strFullName);
			nTotal += GetPhoneImageSpace(strFullName);
		}
		else if(!S_ISDIR(filestat.st_mode)&&(S_ISREG(filestat.st_mode))&&(IsPicture(strFullName, &dFileType)))
		{
			//unlink(strFullName);
			nTotal += filestat.st_size;
		}
		free(strFullName);
		strFullName = NULL;
		dirinfo = readdir(dirtemp);
	}
	
	closedir(dirtemp);

	return nTotal;
	
}
int	GetMMCImageSpace()
{
	return GetPhoneImageSpace("/mnt/fat16/pictures");
}

DWORD	GetFreeMemory()
{	
	int fd;
	DWORD free_mem;
	fd=open("/dev/info",O_RDONLY);
	if (fd == -1)
	{
		return 0;
	}
	ioctl(fd,INFO_IOC_GETMEMMFREE,&free_mem);
	close(fd);
	return free_mem/1024;
}
/*********************************************************************\
* Function	   
* Purpose      Reset game
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
void ResetGame()
{
	DeleteDirectory("/mnt/flash/game/", TRUE);
	mkdir("/mnt/flash/game/", 0666);
}

void ResetFavorite()
{
	unlink("/mnt/flash/favorite/favor.ini");
}


/*********************************************************************\
* Function	   RestroreWldClkDefault
* Purpose      世界时钟：默认设置本地远地都是北京
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static void RestroreWldClkDefault()
{
	unlink("/mnt/flash/WldClock.dat");

}



/*********************************************************************\
 * Function	  ResetDeskSetup 
 * Purpose    复位个性化设置：背景，颜色 
 * Params	   
 * Return	 	   
 * Remarks	   
**********************************************************************/

static void ResetDeskSetup()
{
  /*  FS_WritePrivateProfileString ( SN_BGSCREEN, KN_BG, 
        "ROM:desk1.bmp", SetupFileName );
    
    FS_WritePrivateProfileInt ( SN_COLORCASE, KN_CASENAME, 
        (long)1 , SetupFileName ); 
    FS_WritePrivateProfileInt(SN_COLORCASE, KN_BARCOLOR,
        STATICBARCOLOR, SetupFileName);
    FS_WritePrivateProfileInt(SN_COLORCASE, KN_WINCOLOR,
        CAPTIONCOLOR, SetupFileName);
    FS_WritePrivateProfileInt(SN_COLORCASE, KN_FOCUSCOLOR,
        FOCUSCOLOR, SetupFileName);
*/    
    DlmNotify( PS_SETDESKTOP, 0);

}


/*********************************************************************\
* Function	   RestroreWldClkDefault
* Purpose      浏览器复位
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static void WMLBOR_RESET()
{
	unlink("/mnt/flash/WIECFG3.cfg");
}




/*********************************************************************\
* Function	IsFlashAvailable   
* Purpose   得到当前的自由空间，判断与给定值的关系   
* Params	nSize:给定的所需要的空间大小,单位是 KB  
* Return	如果当前自由空间大于给定值，返回“可以使用”
            如果当前自由空间+碎片空间大于给定值，返回“需要整理”
            如果还小于给定值，返回“需要删除文件”
            失败返回判断失败 

* Remarks	   
**********************************************************************/
int IsFlashAvailable(int nSize)
{
	struct statfs  *spaceinfo = NULL;
	int ret = -1;
	int	nAvailSpace;

	
	spaceinfo = malloc(sizeof(struct statfs));
	if(spaceinfo == NULL)
		return ret;

	memset(spaceinfo, 0 , sizeof(struct statfs) );

	statfs(PHONEFLASH, spaceinfo);

	nAvailSpace = spaceinfo->f_bavail* spaceinfo->f_bsize/1024;

	if(( WARNSPACE+nSize > nAvailSpace)&&(nAvailSpace > ERRSPACE+nSize))
	{
		PLXTipsWin(NULL, NULL, 0, ML("SPACEPROMPT"), ML("Prompt"), Notify_Info, ML("Ok"), NULL, WAITTIMEOUT);
		ret = SPACE_NEEDCLEAR;
	}
	else if(nAvailSpace <= ERRSPACE+nSize)
	{
		PLXTipsWin(NULL, NULL, 0, ML("SPACEWARNING"), ML("Warning"), Notify_Alert, ML("Ok"), NULL, WAITTIMEOUT);
		ret =  SPACE_CHECKERROR;
	}
	else
		ret = SPACE_AVAILABLE;

	free(spaceinfo);
	spaceinfo = NULL;

	return ret;


}


/*********************************************************************\
* Function	IsMMCAvailable   
* Purpose   得到当前的自由空间，判断与给定值的关系   
* Params	nSize:给定的所需要的空间大小,单位是 KB  
* Return	如果当前自由空间大于给定值，返回“可以使用”
            如果当前自由空间+碎片空间大于给定值，返回“需要整理”
            如果还小于给定值，返回“需要删除文件”
            失败返回判断失败 

* Remarks	   
**********************************************************************/
/*
int IsMMCAvailable( int nSize )
{
	FREESPACE		pFree;
	int				nFreeBytes, nWasteBytes, nNeedBytes;

//	int             nSMSNeedBytes, nSMSUsedBytes, nSMSMaxBytes; //预留短信大小
//	int      	    nMMSNeedBytes, nMMSUsedBytes, nMMSMaxBytes; //彩信
//	int   		    nPBNeedBytes, nPBUsedBytes, nPBMaxBytes;    //联系人

//	return SPACE_AVAILABLE;

/ *
	nSMSUsedBytes = GetSMSSpace();
	nSMSMaxBytes  = GetMaxSMSSpace();
	nSMSNeedBytes = nSMSMaxBytes - nSMSUsedBytes;

	nMMSUsedBytes = MMS_GetUsedSpace();
	nMMSMaxBytes  = MMSMAXSIZE;
	nMMSNeedBytes = nMMSMaxBytes - nMMSUsedBytes;

	nPBUsedBytes  = APP_GetPhoneBookFileSize();
	nPBMaxBytes   = PBMAXSIZE;
	nPBNeedBytes  = nPBMaxBytes - nPBUsedBytes;* /


    if(nSize <= 0)
        return SPACE_CHECKERROR;

   //  nNeedBytes = nSize * 1024;
	nNeedBytes = (nSize) * 1024 ;

	/ * 自由空间 * /
	if(!GetFreeSpace("FAT16:", &pFree))
		return SPACE_CHECKERROR;

	/ * 自由空间的大小 = 自由Cluster的个数 * 每个Cluster的Sector的个数 * 每个Sector的字节数 * /
	nFreeBytes = pFree.dwNumberOfFreeClusters 
		 * pFree.dwSectorsPerCluster * pFree.dwBytesPerSector;

    //如果自由空间 > 给定值
    if( nFreeBytes > nNeedBytes )
        return SPACE_AVAILABLE;
    else
    {       
        // 碎片空间的大小 = 碎片Cluster的个数 * 每个Cluster的Sector的个数 * 每个Sector的字节数
        nWasteBytes = pFree.dwNumberOfWasteClusters
            * pFree.dwSectorsPerCluster * pFree.dwBytesPerSector;
        
        // 自由空间 + 碎片空间 > 给定值
        if( (nFreeBytes + nWasteBytes) > nNeedBytes ) 
            return SPACE_NEEDCLEAR;
        else
            return SPACE_NEEDDELFILE;
        
    }
	return SPACE_CHECKERROR;
}*/




/*********************************************************************\
* Function	FileCanDownloadViaWap   
* Purpose   判断文件类型，看是否可通过wap下载   
* Params	char *pszExtFilename 传过来的是文件的扩展名（不带小数点），
            当前支持扩展名类型为mmf,bmp或者wvg
            int  nFileSize       将要下载的文件大小, 单位byte
* Return	-2： 不支持此格式的文件
            -1：FLASH2空间已满,不能下载
            0：此类文件个数已到最大,不能再下载
            1：可以把该文件保存到FLASH2.里
**********************************************************************/
int FileCanDownloadViaWap(char *pszeExtFilename, int nFileSize)
{
    PSTR  CanDownloadExtFilename[10] = {"gif", "jpg", "bmp", "wbmp", "png", "wml" , "wmlc" , "mmf" , "mid" , "amr"};
	//PSTR  CanDownloadExtFilename[9] = {"gif", "jpg", "bmp", "wbmp", "png", "wml" , "wmlc" , "mid" , "amr"};

    int i;

    if(pszeExtFilename == NULL)
	{
		return -2;
	}
        
    for(i=0;i<10;i++)
	//for(i=0;i<9;i++)
    {
        if(!strcmp(pszeExtFilename,CanDownloadExtFilename[i]))
        {
            if(IsFlashAvailable( nFileSize/1024 +100 ) == SPACE_AVAILABLE)
                return 1;
            else
                return -1;
        }
    }
    return -2;
}

char *itoa(int a, char *p, int b)
{
  if(b == 10)
    sprintf(p, "%d", a);
  else if(b == 16)
    sprintf(p, "%x", a);

	return p;
}

/*
char *strupr(char *a)
{
	char *ret;
	
	ASSERT(a != NULL);
	
	ret = a;
	
	while (*a != '\0')
	{
		if (islower ((int)*a))
			*a = toupper (*a);
		++a;
	}
	
	return ret;
}*/



/*
int strnicmp(const char *s1, const char *s2, int n)
{
  return strncasecmp(s1, s2, n);
}
*/
