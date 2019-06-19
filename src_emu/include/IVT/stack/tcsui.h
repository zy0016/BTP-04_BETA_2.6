/* TCS UI header file
Modified History:
2000/1/11 	jc	changed to async UI definition
2000/1/19	jc	change indication defination
*/
#ifndef TCSUI_H
#define TCSUI_H

#define LINK_KEY_LEN		         16

/* Define events from TCS to uplayer */ 
#define PHONE_TCS_CCID_IND                       	0
#define PHONE_TCS_ALERTING_IND	                	1
#define PHONE_TCS_PROGRESS_IND	          	2
#define PHONE_TCS_SETUP_IND                     	3
#define PHONE_TCS_SETUPCOMPLETE_IND      	4
#define PHONE_TCS_DISCONN_IND                 	5
#define PHONE_TCS_LINKLOSS_IND                	6
#define PHONE_TCS_STARTDTMF_IND             	7
#define PHONE_TCS_STARTDTMF_CFM            	8
#define PHONE_TCS_STOPDTMF_IND              	9
#define PHONE_TCS_STOPDTMF_CFM             	10
#define PHONE_TCS_INFO_IND                      	11
#define PHONE_TCS_SETUP_ACK_IND		  	12
#define PHONE_TCS_CALL_PROCEEDING_IND  	13
#define PHONE_TCS_REGISTER_RECALL_IND  	14
#define PHONE_TCS_TLADDED		                 	15
#define PHONE_TCS_INTERCOM_LINK_ADDED   	16 
#define PHONE_TCS_EXIT_PAGESCAN_IND	    	17
#define PHONE_TCS_PROGRESS_COMP_IND	    	18

#define PHONE_TCS_ACCESS_RIGHTS_IND      	30
#define PHONE_TCS_ACCESS_RIGHTS_CFM     	31
#define PHONE_TCS_FAST_ACCESS_IND          	32
#define PHONE_TCS_FAST_ACCESS_CFM         	33
#define PHONE_TCS_INFO_SUGGEST_IND	    	34

/* For TCS Tester */
#define	PHONE_TCS_PROGRESS_IND_EX	      	50
#define	PHONE_TCS_SETUP_IND_EX		      	51
#define	PHONE_TCS_DISCONN_IND_EX	     	52
#define	PHONE_TCS_INFO_IND_EX		      	53

#define TCS_HANDLE 	UCHAR *

/* anyunbo 2004-5-11*/
#define TCS_ROLE_GW	1
#define TCS_ROLE_TL		2

#include "tcsuistru.h"
#include "tcsuifunc.h"

#endif
