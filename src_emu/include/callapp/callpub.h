

#ifndef _CALL_PUB_H
#define _CALL_PUB_H
typedef enum
{
	initif = 0,
	dialupif,
	diallinkif,
	callingif,
	ringif,
	callendif,
	gsmshowif
}calliftype;

calliftype GetCallIf();

BOOL    APP_CallPhone(const char * pPhoneNumber);

#endif 
