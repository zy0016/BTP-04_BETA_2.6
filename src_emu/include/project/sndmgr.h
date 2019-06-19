
#ifndef _SOUND_MANAGE_H_
#define _SOUND_MANAGE_H_

#define SPM_SETCURSEL	(WM_USER+1010)
#define SPM_SETVOLUME	(WM_USER+1011)
#define SPM_SETDEFTONE	(WM_USER+1016)

long	SND_StatMMCFolder();
int		GetDefaultRingEx(char *pFilePath);
HWND	PreviewSoundEx(HWND hFrame, HWND hWnd, UINT returnmsg, PSTR psCaption, BOOL bShowBeneSnd);

#endif	// _SOUND_MANAGE_H_
