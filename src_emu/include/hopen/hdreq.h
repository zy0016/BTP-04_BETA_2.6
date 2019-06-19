
#ifndef _HDREQ_H
#define _HDREQ_H

struct cf_geo{
	unsigned short curncylis;//Current numbers of cylinders
	unsigned short curnheads;//Current numbers of heads
	unsigned short cursecpt;//Current sectors per track
	unsigned int   curnsectors;//Current capacity in sectors (LBAs)
};
#ifndef mc68000
struct mmc_geo {
	unsigned char spec_vers;
	unsigned char tran_speed;
	unsigned char file_format;
	unsigned char ununsed;
	unsigned int  curnsectors; 
};
#else
struct mmc_geo{
	unsigned short curncylis;//Current numbers of cylinders
	unsigned short curnheads;//Current numbers of heads
	unsigned short cursecpt;//Current sectors per track
	unsigned int   curnsectors;//Current capacity in sectors (LBAs)
};
#endif
#endif
