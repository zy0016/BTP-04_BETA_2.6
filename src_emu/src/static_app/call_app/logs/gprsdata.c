#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "gprsdata.h"
#include "RcCommon.h"

#define  GPRSDATAPATH  "/mnt/flash/calllogs/gprsdata.dat"
int FindGPRSData(GPRSDATA * gprsdata)
{
    FILE * fp;
//    char filepath[50] ;  

//	strcpy(filepath, GPRSDATAPATH);
	if ((fp = fopen(GPRSDATAPATH, "r+b")) == NULL)
	{
		fp = fopen(GPRSDATAPATH, "w+b");
		if (fp == NULL) 
			return RTN_GPRS_FAILURE;
		else 
		{
			memset(gprsdata, 0, sizeof(GPRSDATA));
			fseek(fp, 0, SEEK_SET);
			if (fwrite(gprsdata, 1, sizeof(GPRSDATA), fp) != sizeof(GPRSDATA)) 
			{
				fclose(fp);
				return RTN_GPRS_FAILURE;
			}
			else
			{
				fclose(fp);
				return RTN_GPRS_SUCCESS;
			}
		}
	}
	else {
		if (fread(gprsdata, sizeof(GPRSDATA), 1, fp) != 1) 
		{
			fclose(fp);
			return RTN_GPRS_FAILURE;
		}
		else
		{
			fclose(fp);
			return RTN_GPRS_SUCCESS;
		}
	}
}

int AddGPRSData(unsigned long senddata, unsigned long receivedata)
{
	GPRSDATA gprsdata;
	FILE * fp;
//    char filepath[50] ;  
	
//	strcpy(filepath, GPRSDATAPATH);
	if (RTN_GPRS_SUCCESS == FindGPRSData(&gprsdata)) 
	{
		gprsdata.lastsd = senddata;
		gprsdata.lastrd = receivedata;
		gprsdata.rdtotal = gprsdata.rdtotal+receivedata;
		gprsdata.sdtotal = gprsdata.sdtotal+senddata;
		if ((fp = fopen(GPRSDATAPATH, "r+b")) == NULL)
			return RTN_GPRS_FAILURE;
		else
		{
			fseek(fp, 0, SEEK_SET);
			if (fwrite(&gprsdata, 1, sizeof(GPRSDATA), fp) != sizeof(GPRSDATA)) 
			{
				fclose(fp);
				return RTN_GPRS_FAILURE;
			}
			else
			{
				fclose(fp);
				UpdateGPRSCounters();
				return RTN_GPRS_SUCCESS;
			}			
		}
	}
	else
		return RTN_GPRS_FAILURE;
}

int ResetData(void)
{
	GPRSDATA gprsdata;
	FILE * fp;
//    char filepath[50] ;  
	
//	strcpy(filepath, GPRSDATAPATH);
	if (RTN_GPRS_SUCCESS == FindGPRSData(&gprsdata)) 
	{
		memset(&gprsdata, 0, sizeof(GPRSDATA));
		if ((fp = fopen(GPRSDATAPATH, "r+b")) == NULL)
			return RTN_GPRS_FAILURE;
		else
		{
			fseek(fp, 0, SEEK_SET);
			if (fwrite(&gprsdata, 1, sizeof(GPRSDATA), fp) != sizeof(GPRSDATA)) 
			{
				fclose(fp);
				return RTN_GPRS_FAILURE;
			}
			else
			{
				fclose(fp);
				return RTN_GPRS_SUCCESS;
			}			
		}
	}
	else
		return RTN_GPRS_FAILURE;
}
