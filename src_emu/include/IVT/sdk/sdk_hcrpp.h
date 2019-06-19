#ifndef SDK_HCRPP_H
#define SDK_HCRPP_H

BTSVCHDL Btsdk_RegisterHCRPPService(BTUINT16 svc_class);
BTUINT32 Btsdk_UnregisterHCRPPService(BTSVCHDL svc_hdl);
BTUINT8* Btsdk_HCRPPGet1284ID(BTCONNHDL conn_hdl);
#endif
