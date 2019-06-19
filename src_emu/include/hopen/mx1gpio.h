

#ifndef  __MX1GPIO_H__
#define  __MX1GPIO_H__

#define INT_GPIO_PORTA	11
#define INT_GPIO_PORTB	12
#define INT_GPIO_PORTC	13
#define INT_GPIO_PORTD	62

/* input */
/**********button*****************/
#define	SIDE_KEY_UP		31//PORT D
#define SIDE_KEY_DN		15//PORT B
#define DOCK_SW_A		20//PORT A
#define DOCK_SW_B		19//PORT A
#define DOCK_SW_START	 1//PORT A
#define DOCK_SW_SELECT	 6//PORT C
#ifdef _EVT4_
#define MAIN_SNAP_SW	 7//PORT C
#else
#define MAIN_SNAP_SW	17//PORT A
#endif
#define MAIN_TALK_SW	30//PORT D
#define MAIN_HOME_SW	29//PORT D
#define MAIN_PWR_SW		22//PORT D
#define NAV_ENTER		28//PORT D
#define NAV_1			27//PORT D
#define NAV_2			26//PORT D
#define NAV_3			25//PORT D
#define NAV_4			24//PORT D

/**********state*****************/
#define USB_DET			17//PORT D
#define FLIP_CLOSED		16//PORT D
#define DOCK_ON			15//PORT D
#define AC_IN			14//PORT D
#define CHARGE			13//PORT D
#define BATTERY_NG		12//PORT D
#define GSM_RI			 9//PORT D
#define GSM_DCD			 8//PORT D

/* output */
#define PS_PWR_DN		11//PORT D
#define MOTOR			 0//PORT A

typedef unsigned long UINT32;
void set_gpio_output(UINT32 portaddress, UINT32 bit, UINT32 value);
void set_gpio_input(UINT32 portaddress, UINT32 bit);
void set_gpio_interrupt(UINT32 portaddress, UINT32 bit, UINT32 edge);
void set_gpio_pullup(UINT32 portaddress, UINT32 bit, UINT32 value);

#endif
