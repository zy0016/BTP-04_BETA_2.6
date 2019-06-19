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

#define CHECKTXTMAXLEN      10//��ѡ��ť����ʾ�������ֵ
#define OPTIONCLEWMAXLEN    30//��ʾ�ڴ����е���ʾ�������ֵ
#define OPENMAXLEN          10
#define CLOSEMAXLEN         10
typedef enum
{
    RADIOBUTTON,    //��ʾ������ѡ��ť(�򿪺͹ر�)
    CHECKBOXBUTTON, //��ʾһ����ѡ��ť
    STRSPINBOX      //��ʾһ��StrSpinBox�ؼ�(�򿪺͹ر�)
}CONTROLTYPE;
typedef enum
{
    OPTION_TOP      //��ʾ�����ڿؼ����ϱ�
}CLEWPLACE;
typedef enum
{
    POSITION_ROW,   //������ѡ��ť����һ��
    POSITION_COL    //������ѡ��ť����һ��
}RADIOPLACE;
typedef struct 
{
    HWND        hCallWnd;                       //�ص�ģ���ָ�����ھ��
    int         hCallMsg;                       //�ص�ģ���ָ����Ϣ��Ӧ
    char        titlecaption[CAPTIONMAXLEN + 1];//���ڵı���
    CONTROLTYPE ControlType;                    //��Ҫ��ʾ�Ŀؼ�����
    BOOL        ControlValue;                   //ʹ��������ؿؼ�ѡ�е�ֵ(�������0)
    char        CheckTxt[CHECKTXTMAXLEN + 1];   //��ѡ��ť����ʾ����.
    char        cOpen[OPENMAXLEN + 1];
    char        cClose[CLOSEMAXLEN + 1];
    char        Clew[OPTIONCLEWMAXLEN + 1];     //��ʾ�ڴ����е���ʾ����(���п���)
    CLEWPLACE   ClewPosition;                   //Clew�����StrSpinBox�ؼ��͸�ѡ��ť��λ��
//(iPositionδָ����Clew�ڿؼ������,Clew�ĳ���Ҫ��С,���������ʾ��ȫ,��һ��Ҫ����Ա����)
    RADIOPLACE  RadioPosition;                  //������ѡ��ť�����λ��
}OPTIONSELECT;
BOOL    CallOptionWindow(const OPTIONSELECT * pOptionSelect);
/*
CallOptionWindow������ʾһ������,��������ʾ��3��ؼ�,�ؼ���������ControlType��������
ControlType = RADIOBUTTON:��������ʾ��������ѡ��ť,��Ӧ������Ϊ"��"��"�ر�"
    ��ControlValueΪTUREʱ,��ѡ��ť��"��"�ᱻѡ��.
    ��ControlValueΪFALSEʱ,��ѡ��ť��"�ر�"�ᱻѡ��.
ControlType = CHECKBOXBUTTON:���ڻ���ʾһ����ѡ��ť,��Ӧ�����ְ�����CheckTxt��.
    ��ControlValueΪTRUEʱ,��ѡ��ť�ᱻѡ��.���򲻻�ѡ��.
ControlType = STRSPINBOX:���ڻ���ʾһ��StrSpinBox�ؼ�,�ؼ��а�������ѡ��,����Ϊ"��"��"�ر�"
    ��ControlValueΪTRUE,StrSpinBox�ؼ�����ʾ"��"ѡ��.
    ��ControlValueΪFALSEʱ,StrSpinBox�ؼ�����ʾ"�ر�"ѡ��.

�û�ѡ�񴰿ڵ�"ȷ��"��ť֮��
WPARAM=1
ControlType = RADIOBUTTON:
���ѡ��"��"��ť,LPARAM = 1;���ѡ��"�ر�"��ť,LPARAM = 0
ControlType = CHECKBOXBUTTON:
���ѡ���˸�ѡ��ť,LPARAM = 1;���û��ѡ��ѡ��ť,LPARAM = 0
ControlType = STRSPINBOX:
���ѡ��"��"��ť,LPARAM = 1;���ѡ��"�ر�"��ť,LPARAM = 0

�û�ѡ�񴰿ڵ�"ȡ��"��ť֮��,�ص���Ϣ�е�WPARAM=0,LPARAM = 0
*/
#endif
