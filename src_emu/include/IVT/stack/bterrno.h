#ifndef BTERRNO_H
#define BTERRNO_H

#define BTERROR_NONE			0

/* 
	Every Error nuber is 1 byte.
	All errors are divided into 4 groups:
	HCI, Stack, profile and application.
	User should call BtGetLastError() to get the error code and use correspoding
	Macros to retrieve the error code of the group.
	Developer should call correspoding Macros to set the error code of the group.
*/

#define BTERROR_HCI_BUSY		0x01

#define BTERROR_STACK_BUSY		0x01

#define BTERROR_PROFILE_BUSY	0x01
#define BTERROR_PAN_BNEP_CONN_FAILED		0x02
#define BTERROR_PAN_BNEP_DISC_FAILED		0x03
#define BTERROR_PAN_REG_PORT_FAILED			0x04
#define BTERROR_PAN_REG_SERVICE_FAILED		0x05	
#define BTERROR_PAN_MEMORY_ALLOC_FAILED		0x06
#define BTERROR_PAN_UNKNOWN_SERVICE_TYPE	0x07
#define BTERROR_PAN_INVALID_PARAMETER		0x08
#define BTERROR_PAN_BUFFER_TOO_LITTER		0x09

#define BTERROR_APP_BUSY		0x01

#endif
