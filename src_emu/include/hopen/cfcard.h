
#ifndef	_CFCARD_H
#define _CFCARD_H
#include <hopen/ioctl.h>

struct  cf_id_info {
	unsigned short signature;//General configuration - signature for the CompactFlash Storage Card
	unsigned short ncylis;//Default number of cylinders
	unsigned short Reserved0;
	unsigned short nheads;//Default number of heads
	unsigned short nufmtpt;//Number of unformatted bytes per track
	unsigned short nufmtps;//Number of unformatted bytes per sector
	unsigned short nsecpt;//Default number of sectors per track
	unsigned int   nsectors;//Number of sectors per card (Word 7 = MSW, Word 8 = LSW)
	unsigned short Vendor;//Vendor Unique
	unsigned char  Serialnum[20];//Serial number in ASCII (Right Justified)
	unsigned short buftype;//Buffer type
	unsigned short bufsize;//Buffer size in 512 byte increments
	unsigned short eccsize;//# of ECC bytes passed on Read/Write Long Commands
	unsigned char  Firm[8];//Firmware revision in ASCII. Big Endian Byte Order in Word
	unsigned char  Modelnum[40];//Model number in ASCII (Left Justified) Big Endian Byte Order in Word
	unsigned short MaxMutinum;//Maximum number of sectors on Read/Write Multiple command
	unsigned short dwordsupport;//Double Word not supported
	unsigned short Capability;//Capabilities
	unsigned short Reserved1;//Reserved
	unsigned short ptiming;//PIO data transfer cycle timing mode
	unsigned short dtiming;//DMA data transfer cycle timing mode
	unsigned short tparam;//Translation parameters are valid
	unsigned short curncylis;//Current numbers of cylinders
	unsigned short curnheads;//Current numbers of heads
	unsigned short cursecpt;//Current sectors per track
	unsigned int   curnsectors;//Current capacity in sectors (LBAs)(Word 57 = LSW, Word 58 = MSW)
	unsigned short mutlisetting;//Multiple sector setting
	unsigned int   lbansectors;//Total number of sectors addressable in LBA Mode
	unsigned char  reser[138];//Reserved
	unsigned short Securstatus;//Security status
	unsigned char  venderunique[64];//Vendor unique bytes
	unsigned short Powerrequire;//Power requirement description
	unsigned char  reserved[170];//Reserved
};

#define CF_IOC_MAGIC		'C'

#define CF_IOCBLKGETSECS		_IOR(CF_IOC_MAGIC,1,struct cf_id_info)
#define CF_IOCBLKGETSECSIZE		_IOR(CF_IOC_MAGIC,2,struct cf_id_info)
#define CF_IOCBLKGETSIZE		_IOR(CF_IOC_MAGIC,3,struct cf_id_info)
#define CF_IOCBLKFLSBUF			_IO(CF_IOC_MAGIC,4)
#define CF_HDIO_GETGEO			_IOR(CF_IOC_MAGIC,5,struct cf_id_info)
#define CF_HDIO_FORMAT			_IOW(CF_IOC_MAGIC,6,struct cf_id_info)

#endif /* _CFCARD_H */