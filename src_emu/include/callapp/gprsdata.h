

#ifndef _GPRSDATA_H
#define _GPRSDATA_H

#define  RTN_GPRS_SUCCESS	 0
#define  RTN_GPRS_FAILURE	 -1 

typedef struct  tagGPRSData{
	unsigned long lastsd;
	unsigned long lastrd;
	unsigned long sdtotal;
	unsigned long rdtotal;
}GPRSDATA;

int FindGPRSData(GPRSDATA * gprsdata);
int AddGPRSData(unsigned long senddata, unsigned long receivedata);
int ResetData(void);

#endif
