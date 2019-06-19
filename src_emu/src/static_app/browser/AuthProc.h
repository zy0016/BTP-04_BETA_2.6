/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : 
 *
 * Purpose  : 
 *
\**************************************************************************/

#ifndef AUTHPROC_H
#define AUTHPROC_H

extern BOOL WIE_WapAuth(char *szName, int nMaxName, char *szPsw, int nMaxPsw,
				 const char *PREALM, int nRealmLen, const char *PROOT, int Rootlen, int type);
extern BOOL WIE_WapPermit(const char *SHOWINFOR, int inforlen, const char *CHAR1, int c1len,
				   const char *CHAR2, int c2len);
extern BOOL WIE_WapPrompt(const char *PTITLE, int nTitleLen, 
				   const char* PDEFRESULT, int deflen,
				   char* result, int *nReslen);
extern BOOL WIE_WapAssert(const char *pMsg, int nMsgLen);

extern void WIE_WapDealJad(const char *CREQURL, const char *HTTP_HEAD, unsigned int headlen,
                 const char *HTTP_CONTENT, unsigned int contentlen);
extern BOOL WIE_MailTo(char *szMail, char *szCc, char *szBcc,
                       char *szSubject,char *szBody);

#endif
