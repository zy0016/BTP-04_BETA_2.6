#include    "window.h"
#include    "string.h"
#include    "stdlib.h"
#include    "stdio.h" 
#include    "winpda.h"
#include    "plx_pdaex.h"
#include    "setting.h"
#include    "pubapp.h"
#include    "setup.h"
#include    "mullang.h"

#define SN_MODEM_STATE "ModemStateSetting"
#define KN_USB_MODEM   "USBModemStateSetting"
#define KN_BT_MODEM    "BTModemStateSetting"


void Set_USB_Modem_State(SWITCHTYPE OnOrOff);
void Set_BT_Modem_State(SWITCHTYPE OnOrOff);

void Set_USB_Modem_State(SWITCHTYPE OnOrOff)
{
    FS_WritePrivateProfileInt(SN_MODEM_STATE, KN_USB_MODEM, OnOrOff, SETUPFILENAME);

    if(OnOrOff == SWITCH_ON)
        Set_BT_Modem_State(SWITCH_OFF);
}

void Set_BT_Modem_State(SWITCHTYPE OnOrOff)
{
    FS_WritePrivateProfileInt(SN_MODEM_STATE, KN_BT_MODEM, OnOrOff, SETUPFILENAME);
    
    if(OnOrOff == SWITCH_ON)
        Set_USB_Modem_State(SWITCH_OFF);
}

SWITCHTYPE Get_USB_Modem_State(void)
{
    return FS_GetPrivateProfileInt(SN_MODEM_STATE, KN_USB_MODEM, SWITCH_ON, SETUPFILENAME);
}

SWITCHTYPE Get_BT_Modem_State(void)
{
    return FS_GetPrivateProfileInt(SN_MODEM_STATE, KN_BT_MODEM, SWITCH_OFF, SETUPFILENAME);
}
