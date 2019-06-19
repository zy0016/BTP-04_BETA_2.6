#ifndef _SDK_WRAP_DEP_H
#define _SDK_WRAP_DE_H

BTINT32 Btsdk_InitCommObj(BTUINT8 com_idx, BTUINT16 svc_class);
BTINT32 Btsdk_DeinitCommObj(BTUINT8 com_idx);
void Btsdk_WrapSetCallback(void *func);
void Btsdk_WrapSetOpenCompleteEvent(BTUINT8 com_idx);
void Btsdk_WrapResetAllOpenCompleteEvent(void);

#endif
