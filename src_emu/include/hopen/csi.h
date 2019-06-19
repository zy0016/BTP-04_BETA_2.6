
#ifndef		__CSI_H__
#define		__CSI_H__

#define		FMT_VGA		0
#define		FMT_QVGA	1
#define		FMT_CIF		2
#define		FMT_QCIF	3
#define		FMT_QQVGA	4

#define		CSCTL_FMT		_IOW('S', 0, int)
#define		CSCTL_VIEWFINDER	_IOW('S', 1, int)
#define		CSCTL_CAPTURE		_IOW('S', 2, int)
#define		CSCTL_GETADDR		_IOW('S', 3, int)
#define		CSCTL_ZOOMINx2		_IOW('S', 4, int)
#define		CSCTL_ZOOMOUTx2		_IOW('S', 5, int)
#define 	CSCTL_WINSIZE		_IOW('S', 6, int)
#define		CSCTL_VIDEO		_IOW('S', 7, int)
#define		CSCTL_UNLOCK		_IOW('S', 8, int)

//the paramter width of CSCTL_WINSIZE must be less or equal to WIDTH_MAX
//and the paramter height of CSCTL_WINXY must be less or equal to HEIGHT_MAX
#define 	WIDTH_MAX		320
#define 	HEIGHT_MAX		240


struct frame_info {
	int pos;
	int count;
	int frames;
	unsigned int addr;
};



#endif 

