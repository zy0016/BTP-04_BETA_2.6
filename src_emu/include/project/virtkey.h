/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : Window
 *
 * Purpose  : Define virtual key code.
 *            
\**************************************************************************/

#ifndef __VIRTKEY_H
#define __VIRTKEY_H

//      Name            Value(hex)      Mouse or keyboard equivalent

#define VK_LBUTTON      0x01            // Left mouse button 
#define VK_RBUTTON      0x02            // Right mouse button 
#define VK_CANCEL       0x03            // Control-break processing 
#define VK_MBUTTON      0x04            // Middle mouse button (three-button mouse) 

#define VK_BACK         0x08            // BACKSPACE key 
#define VK_TAB          0x09            // TAB key 

#define VK_CLEAR        0x0C            // CLEAR key
#define VK_RETURN       0x0D            // ENTER key 

#define VK_SHIFT        0x10            // SHIFT key 
#define VK_CONTROL      0x11            // CTRL key 
#define VK_MENU         0x12            // ALT key 
#define VK_ALT          0x12            // ALT key     
#define VK_PAUSE        0x13            // PAUSE key 
#define VK_CAPITAL      0x14            // CAPS LOCK key 

#define VK_ESCAPE       0x1B            // ESC key 

#define VK_SPACE        0x20            // SPACEBAR 
#define VK_PRIOR        0x21            // PAGE UP key 
#define VK_PAGEUP       VK_PRIOR
#define VK_NEXT         0x22            // PAGE DOWN key 
#define VK_PAGEDOWN     VK_NEXT
#define VK_END          0x23            // END key 
#define VK_HOME         0x24            // HOME key 
#define VK_LEFT         0x25            // LEFT ARROW key 
#define VK_UP           0x26            // UP ARROW key 
#define VK_RIGHT        0x27            // RIGHT ARROW key 
#define VK_DOWN         0x28            // DOWN ARROW key 
#define VK_SELECT       0x29            // SELECT key 

#define VK_EXECUTE      0x2B            // EXECUTE key 
#define VK_SNAPSHOT     0x2C            // PRINT SCREEN key for Windows 3.0 and later 
#define VK_INSERT       0x2D            // INS key 
#define VK_DELETE       0x2E            // DEL key 
#define VK_HELP         0x2F            // HELP key 

/* VK_0 thru VK_9 are the same as ASCII '0' thru '9' (0x30 - 0x39) */
/* VK_A thru VK_Z are the same as ASCII 'A' thru 'Z' (0x41 - 0x5A) */

#define VK_0            0x30            // 0 key, same as '0'
#define VK_1            0x31            // 1 key, same as '1' 
#define VK_2            0x32            // 2 key, same as '2' 
#define VK_3            0x33            // 3 key, same as '3' 
#define VK_4            0x34            // 4 key, same as '4' 
#define VK_5            0x35            // 5 key, same as '5' 
#define VK_6            0x36            // 6 key, same as '6' 
#define VK_7            0x37            // 7 key, same as '7' 
#define VK_8            0x38            // 8 key, same as '8' 
#define VK_9            0x39            // 9 key, same as '9' 
                                                           
#define VK_A            0x41            // A key 
#define VK_B            0x42            // B key 
#define VK_C            0x43            // C key 
#define VK_D            0x44            // D key 
#define VK_E            0x45            // E key 
#define VK_F            0x46            // F key 
#define VK_G            0x47            // G key 
#define VK_H            0x48            // H key 
#define VK_I            0x49            // I key 
#define VK_J            0x4A            // J key 
#define VK_K            0x4B            // K key 
#define VK_L            0x4C            // L key 
#define VK_M            0x4D            // M key 
#define VK_N            0x4E            // N key 
#define VK_O            0x4F            // O key 
#define VK_P            0x50            // P key 
#define VK_Q            0x51            // Q key 
#define VK_R            0x52            // R key 
#define VK_S            0x53            // S key 
#define VK_T            0x54            // T key 
#define VK_U            0x55            // U key 
#define VK_V            0x56            // V key 
#define VK_W            0x57            // W key 
#define VK_X            0x58            // X key 
#define VK_Y            0x59            // Y key 
#define VK_Z            0x5A            // Z key 

#define VK_LWIN         0x5B
#define VK_RWIN         0x5C
#define VK_APPS         0x5D

#define VK_NUMPAD0      0x60            // Numeric keypad 0 key 
#define VK_NUMPAD1      0x61            // Numeric keypad 1 key 
#define VK_NUMPAD2      0x62            // Numeric keypad 2 key 
#define VK_NUMPAD3      0x63            // Numeric keypad 3 key 
#define VK_NUMPAD4      0x64            // Numeric keypad 4 key 
#define VK_NUMPAD5      0x65            // Numeric keypad 5 key 
#define VK_NUMPAD6      0x66            // Numeric keypad 6 key 
#define VK_NUMPAD7      0x67            // Numeric keypad 7 key 
#define VK_NUMPAD8      0x68            // Numeric keypad 8 key 
#define VK_NUMPAD9      0x69            // Numeric keypad 9 key 
#define VK_MULTIPLY     0x6A            // Multiply key 
#define VK_ADD          0x6B            // Add key 
#define VK_SEPARATOR    0x6C            // Separator key 
#define VK_SUBTRACT     0x6D            // Subtract key 
#define VK_DECIMAL      0x6E            // Decimal key 
#define VK_DIVIDE       0x6F            // Divide key 
#define VK_F1           0x70            // F1 key 
#define VK_F2           0x71            // F2 key 
#define VK_F3           0x72            // F3 key 
#define VK_F4           0x73            // F4 key 
#define VK_F5           0x74            // F5 key 
#define VK_F6           0x75            // F6 key 
#define VK_F7           0x76            // F7 key 
#define VK_F8           0x77            // F8 key 
#define VK_F9           0x78            // F9 key 
#define VK_F10          0x79            // F10 key 
#define VK_F11          0x7A            // F11 key 
#define VK_F12          0x7B            // F12 key 
#define VK_F13          0x7C            // F13 key 
#define VK_F14          0x7D            // F14 key 
#define VK_F15          0x7E            // F15 key 
#define VK_F16          0x7F            // F16 key 
#define VK_F17          0x80            // F17 key 
#define VK_F18          0x81            // F18 key 
#define VK_F19          0x82            // F19 key 
#define VK_F20          0x83            // F20 key 
#define VK_F21          0x84            // F21 key 
#define VK_F22          0x85            // F22 key 
#define VK_F23          0x86            // F23 key 
#define VK_F24          0x87            // F24 key 

#define VK_NUMLOCK      0x90            // NUM LOCK key 
#define VK_SCROLL       0x91            // SCROLL LOCK key 

/*
 * VK_L* & VK_R* - left and right Alt, Ctrl and Shift virtual keys.
 * Used only as parameters to GetAsyncKeyState() and GetKeyState().
 * No other API or message will distinguish left and right keys in this way.
 */
#define VK_LSHIFT       0xA0
#define VK_RSHIFT       0xA1
#define VK_LCONTROL     0xA2
#define VK_RCONTROL     0xA3
#define VK_LALT         0xA4
#define VK_RALT         0xA5

#define VK_LMENU        0xA4
#define VK_RMENU        0xA5

// Virtual key for PalmPilot PDA panel
#define VK_PROGMAN      0xA0
#define VK_KBD1         0xA2
#define VK_KBD2         0xA3
#define VK_CALC         0xA4
#define VK_FIND         0xA5
#define VK_KEY1         0xA6
#define VK_KEY2         0xA7
#define VK_KEY3         0xA8
#define VK_KEY4         0xA9

// Additional virtual key for PDA
#define VK_WAVE         0xC0            // '`' AND '~'
#define VK_MINUS        0xBD            // MINUS KEY
#define VK_EQUAL        0xBB            // EQUAL KEY
#define VK_LBRACKET     0xDB            // LEFT BRACKET
#define VK_RBRACKET     0xDD            // RIGHT BRACKET
#define VK_SEMICOLON    0xBA            // SEMICOLON KEY
#define VK_QUOTA        0xDE            // QUOTATION MARK KEY
#define VK_COMMA        0xBC            // COMMA KEY
#define VK_PERIOD       0xBE            // PERIOD KEY
#define VK_QUESTION     0xBF            // '/' AND '?'
#define VK_SLASH        0xDC            // '\' AND '|'

// Virtual key for stb remote controller

// Esc  key defined as VK_ESCAPE
// Back key defined as VK_BACK
// Menu key defined as VK_MENU
// Up arrow key define as VK_UP
// Left arrow key define as VK_LEFT
// Right arrow key define as VK_RIGHT
// Down arrow key define as VK_DOWN

#define VK_FORWARD      VK_F1

// Enter key defined as VK_ENTER

#define VK_FRAME        VK_F2
#define VK_FONT         VK_F3
#define VK_RELOAD       VK_F4
#define VK_FAVORITES    VK_F5
#define VK_JUMP         VK_F6

// Delete key defined as VK_DELETE
// PageUp key defined as VK_PRIOR
// PageDown key defined as VK_NEXT

#define VK_HOTANS       VK_F7
#define VK_SETUP        VK_F8

// Home key defined as VK_HOEM

#define VK_EMAIL        VK_F9


/* Added on 2004/02/17 for for user defined vk  */
#define VK_SC_54        0xD1
#define VK_SC_55        0xD2
#define VK_SC_56        0xD3

#define VK_SC_59        0xD4
#define VK_SC_5A        0xD5
#define VK_SC_5B        0xD6
#define VK_SC_5C        0xD7
#define VK_SC_5D        0xD8
#define VK_SC_5E        0xD9
#define VK_SC_5F        0xDA
#define VK_SC_60        0xDF
#define VK_SC_61        0xE0
#define VK_SC_62        0xE1
#define VK_SC_63        0xE2
#define VK_SC_64        0xE3
#define VK_SC_65        0xE4
#define VK_SC_66        0xE5
#define VK_SC_67        0xE6
#define VK_SC_68        0xE7
#define VK_SC_69        0xE8
#define VK_SC_6A        0xE9
#define VK_SC_6B        0xEA
#define VK_SC_6C        0xEB
#define VK_SC_6D        0xEC
#define VK_SC_6E        0xED
#define VK_SC_6F        0xEE
#define VK_SC_70        0xEF
#define VK_SC_71        0xF0
#define VK_SC_72        0xF1
#define VK_SC_73        0xF2
#define VK_SC_74        0xF3
#define VK_SC_75        0xF4
#define VK_SC_76        0xF5
#define VK_SC_77        0xF6
#define VK_SC_78        0xF7
#define VK_SC_79        0xF8
#define VK_SC_7A        0xF9
#define VK_SC_7B        0xFA
#define VK_SC_7C        0xFB
#define VK_SC_7D        0xFC
#define VK_SC_7E        0xFD
#define VK_SC_7F        0xFE
#define VK_UNDEFINE		0xFF

/* Added on 2004/02/17 for for PHOSPHOR project */
#define VK_CAMERA_SNAP  VK_SC_60    //MAIN_SNAP_SW	      
#define VK_CHARGER_IN   VK_F11		//VK_SC_61    //CHARGING            
#define VK_CHARGER_OUT  VK_F12		//VK_SC_62    //NOT CHARGING        
#define VK_RINGIN       VK_SNAPSHOT //Modem ring detected 
#define VK_DCD          VK_SC_64    //Modem carrier detect
#define VK_D_START      VK_SC_65    //DOCK_SW_START       
#define VK_D_SELECT     VK_SC_66    //DOCK_SW_SELECT      
#define VK_DOCK_IN      VK_SC_67    //Docked              
#define VK_DOCK_OUT     VK_SC_68    //Not docked          
#define VK_AC_IN        VK_SC_69    //ACIN connected      
#define VK_AC_OUT       VK_SC_6A    //No ACIN             
#define VK_BAT_NG       VK_SC_6B    //Battery level NG    
#define VK_BAT_OK       VK_SC_6C    //Battery level OK    
#define VK_USB_IN       VK_F13		//VK_SC_6D    //USB Connected       
#define VK_USB_OUT      VK_F14		//VK_SC_6E    //USB Disconnected    
#define VK_EARPHONE_OUT VK_F15		//VK_SC_6F    //Headphone unplug    
#define VK_EARPHONE_IN  VK_F16		//VK_SC_70    //Headphone detected  
#define VK_D_A          VK_SC_71    //DOCK_SW_A           
#define VK_D_B          VK_SC_72    //DOCK_SW_B           
#define VK_MMC_IN       VK_SC_73    //MMC PLUG            
#define VK_MMC_OUT      VK_SC_74    //MMC UNPLUG          
#define VK_FLIP_ON      VK_F6       //FLIP ON
#define VK_FLIP_OFF     VK_F7       //FLIP OFF
#define VK_POWEROFF     VK_F2
#define VK_WAKEUP				VK_HELP		//When Wake up by keypad , device send VK_WAKEUP and use code of VK_HELP
#define VK_WAKEUP_KEY			VK_WAKEUP
#define VK_WAKEUP_POWERKEY		VK_NUMPAD0
#define VK_WAKEUP_EMERGENCYKEY	VK_NUMPAD1
#define VK_WAKEUP_EXTERNAL		VK_NUMPAD2
#define VK_WAKEUP_RINGIN		VK_NUMPAD3
#define VK_LOWPOWER				VK_NUMPAD4

#endif
