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
#ifndef _SETUP_OPTIONSELECT_H
#define _SETUP_OPTIONSELECT_H

#define CHECKTXTMAXLEN      10//复选按钮的提示文字最大值
#define OPTIONCLEWMAXLEN    30//显示在窗口中的提示文字最大值
#define OPENMAXLEN          10
#define CLOSEMAXLEN         10
typedef enum
{
    RADIOBUTTON,    //显示两个单选按钮(打开和关闭)
    CHECKBOXBUTTON, //显示一个复选按钮
    STRSPINBOX      //显示一个StrSpinBox控件(打开和关闭)
}CONTROLTYPE;
typedef enum
{
    OPTION_TOP      //提示文字在控件的上边
}CLEWPLACE;
typedef enum
{
    POSITION_ROW,   //两个单选按钮放在一行
    POSITION_COL    //两个单选按钮放在一列
}RADIOPLACE;
typedef struct 
{
    HWND        hCallWnd;                       //回调模块的指定窗口句柄
    int         hCallMsg;                       //回调模块的指定消息响应
    char        titlecaption[CAPTIONMAXLEN + 1];//窗口的标题
    CONTROLTYPE ControlType;                    //需要显示的控件类型
    BOOL        ControlValue;                   //使窗口中相关控件选中的值(非零或者0)
    char        CheckTxt[CHECKTXTMAXLEN + 1];   //复选按钮的提示文字.
    char        cOpen[OPENMAXLEN + 1];
    char        cClose[CLOSEMAXLEN + 1];
    char        Clew[OPTIONCLEWMAXLEN + 1];     //显示在窗口中的提示文字(可有可无)
    CLEWPLACE   ClewPosition;                   //Clew相对于StrSpinBox控件和复选按钮的位置
//(iPosition未指定则Clew在控件的左边,Clew的长度要减小,否则可能显示不全,这一点要程序员控制)
    RADIOPLACE  RadioPosition;                  //两个单选按钮的相对位置
}OPTIONSELECT;
BOOL    CallOptionWindow(const OPTIONSELECT * pOptionSelect);
/*
CallOptionWindow函数显示一个窗口,窗口中显示了3类控件,控件的类型由ControlType变量决定
ControlType = RADIOBUTTON:窗口中显示了两个单选按钮,对应的文字为"打开"和"关闭"
    当ControlValue为TURE时,单选按钮的"打开"会被选中.
    当ControlValue为FALSE时,单选按钮的"关闭"会被选中.
ControlType = CHECKBOXBUTTON:窗口会显示一个复选按钮,对应的文字包含在CheckTxt中.
    当ControlValue为TRUE时,复选按钮会被选中.否则不会选中.
ControlType = STRSPINBOX:窗口会显示一个StrSpinBox控件,控件中包含两个选项,文字为"打开"和"关闭"
    当ControlValue为TRUE,StrSpinBox控件会显示"打开"选项.
    当ControlValue为FALSE时,StrSpinBox控件会显示"关闭"选项.

用户选择窗口的"确定"按钮之后
WPARAM=1
ControlType = RADIOBUTTON:
如果选中"打开"按钮,LPARAM = 1;如果选中"关闭"按钮,LPARAM = 0
ControlType = CHECKBOXBUTTON:
如果选择了复选按钮,LPARAM = 1;如果没有选择复选按钮,LPARAM = 0
ControlType = STRSPINBOX:
如果选择"打开"按钮,LPARAM = 1;如果选择"关闭"按钮,LPARAM = 0

用户选择窗口的"取消"按钮之后,回调消息中的WPARAM=0,LPARAM = 0
*/
#endif
