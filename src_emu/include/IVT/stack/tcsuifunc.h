/*
Modified History
-----------------
		2001/1/11 jc	redefined interface
		2001/2/05 jc	change PHONE_TCSSetupCfm
		2001/3/14 xf	remove unused function
*/

#ifndef TCSUIFUNC_H
#define TCSUIFUNC_H

/* TCS initialize and done */
FUNC_EXPORT	void TcsInit(void);
FUNC_EXPORT	void TcsDone(void);

/* The following functions are called by uplayer, functions with Ex suffix are for TCS FULL users */
typedef void (FuncPHONE_Callback)( UCHAR code, TCS_HANDLE ccid,WORD wparam, DWORD lparam);

/* The function provides method for uplayer to register callback function called by TCS */
FUNC_EXPORT void  TcsCallbackRegister(FuncPHONE_Callback *funcaddr);

//#ifndef CONFIG_TCS_GW
FUNC_EXPORT void	TCS_SignalChannel_Conn_Req(UCHAR*);
FUNC_EXPORT void	TCS_SignalChannel_Disconn_Req(UCHAR*);
//#endif
FUNC_EXPORT void	TCS_L2CAPReleaseReq(WORD);
FUNC_EXPORT void	TCS_SetupReq(struct SetupReqParamStru *);
FUNC_EXPORT void	TCS_SetupRsp(TCS_HANDLE);
FUNC_EXPORT void	TCS_ProgressReq(TCS_HANDLE, struct ProgressStru *);
FUNC_EXPORT void	TCS_DisconnReq(TCS_HANDLE, struct DisconnReqInStru *);
FUNC_EXPORT void	TCS_Start_DTMF_Req(TCS_HANDLE, UCHAR);
FUNC_EXPORT void	TCS_Start_DTMF_Rsp(TCS_HANDLE, UCHAR, UCHAR);
FUNC_EXPORT void	TCS_Stop_DTMF_Req(TCS_HANDLE);
FUNC_EXPORT void	TCS_Stop_DTMF_Ack(TCS_HANDLE, UCHAR);
FUNC_EXPORT void	TCS_InfoReq(TCS_HANDLE, struct CompanySpecStru*);
FUNC_EXPORT void TCS_RegisterRecallReq(TCS_HANDLE ccid);

#ifdef CONFIG_TCS_GM
/* These functions are about group manager. */
FUNC_EXPORT void    TCS_Access_Rights_Req(struct CompanySpecStru*);
FUNC_EXPORT void    TCS_Access_Rights_Rsp(struct GmRightsRspStru*);
FUNC_EXPORT void    TCS_Fast_Access_Req(struct GmFastAccessReqStru*);
FUNC_EXPORT void    TCS_Fast_Access_Rsp(struct GmFastAccessRspStru*);
FUNC_EXPORT void    TCS_Update_Master_Key(void);
#endif

#ifdef CONFIG_TCS_FULL
/* These functions are for full TCS implement. */
FUNC_EXPORT void	TCS_SetupReqEx(struct SetupReqExParamStru *);
FUNC_EXPORT void	TCS_DisconnReqEx(TCS_HANDLE, struct DisconnReqExStru *);
FUNC_EXPORT void    TCS_Setup_Ack_Req(TCS_HANDLE, struct CompanySpecStru*);
FUNC_EXPORT void    TCS_Call_Proceeding_Req(TCS_HANDLE, struct CompanySpecStru*);
FUNC_EXPORT void    TCS_Alerting_Req(TCS_HANDLE, struct CompanySpecStru*);
FUNC_EXPORT void    TCS_InfoReqEx(TCS_HANDLE, struct InfoReqExStru*);
#endif
#endif
