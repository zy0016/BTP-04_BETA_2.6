#ifndef _SDK_FTP_DEF
#define _SDK_FTP_DEF

#ifdef CONFIG_FTP
void FTP_SetBrowseFileInfo(FTP_FIND_DATA *data, BTUINT8 *lpFindFileData); //Change the FTP_FIND_DATA file infomation to the structure depends on the system.
BTINT32 FTP_GetFindDataStruSize(void);
#endif

#endif
