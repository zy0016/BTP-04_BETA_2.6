#ifndef SWRAPUIFUNC_H
#define SWRAPUIFUNC_H

#define DRV_OPEN_PORT	1
#define DRV_CLOSE_PORT	0

typedef void (*SWRAP_Callback)(UCHAR event,UCHAR param);
void SWrap_SetCallback(SWRAP_Callback pProc);

BOOL Swrap_Init(void);
void Swrap_Done(void);

void Swrap_ResetAllOpenCompleteEvent();
void Swrap_SetOpenCompleteEvent(UCHAR com_index);
UCHAR Swrap_GetComIndex(UCHAR vport);
void Swrap_SetCanSendEvent(UCHAR com_index);
void Swrap_Send2SDrvData(UCHAR com_index,UCHAR *Buff,WORD len);
void Swrap_Send2SDrvMdmSigInd(UCHAR com_index,BYTE modemstatus);
void Swrap_Send2SDrvMS_Rsp(UCHAR com_index,BYTE modemstatus);
void Swrap_Send2DrvFromHCRP(DWORD h,UCHAR *Buff,WORD len);
void Swrap_Send2DrvConnLoseInd(UCHAR com_index);

UCHAR Swrap_Bound_Clnt(UCHAR vport,WORD uuid,UCHAR *com_index);
UCHAR Swrap_Bound_Svr(UCHAR vport,WORD uuid,UCHAR *com_index);
void Swrap_UnBound(UCHAR com_index);

#endif
