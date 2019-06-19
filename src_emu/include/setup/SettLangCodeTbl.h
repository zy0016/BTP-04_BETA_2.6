#ifndef _SETTLANGCODETBL_H
#define _SETTLANGCODETBL_H

#include "version.h"

#define MAX_LANG_STR_LEN    15
#define MAX_LANG_CODE_LEN    3


typedef struct tag_MultiLangCode
{
    char chLangStr[MAX_LANG_STR_LEN + 1];       //make sure the string is same as defined in *.ml file
    char nchLangCode[MAX_LANG_CODE_LEN + 1];
}MultiLangCode, *PMultiLangCode;

#ifdef LANGUAGE_CHN
const char DefaultLangStr[] = {"Chinese"}; //Default language if can not find the code;
const MultiLangCode MultiLangCodeTable[] = 
{
    {"English", "234"},
    {"English", "266"},
    {"English", "272"},
    {"English", "302"},
    {"English", "310"},
    {"English", "311"},
    {"English", "316"},
    {"English", "332"},
    {"English", "338"},
    {"English", "344"},
    {"English", "350"},
    {"English", "374"},
    {"English", "544"},
    {"English", "702"},
	{"Chinese", "460"}, 
    {"",""}, //end indicator, do not delete;
};
#else
const char DefaultLangStr[] = {"English"}; //Default language if can not find the code;
const MultiLangCode MultiLangCodeTable[] = 
{
    {"English", "234"},
    {"English", "266"},
    {"English", "272"},
    {"English", "302"},
    {"English", "310"},
    {"English", "311"},
    {"English", "316"},
    {"English", "332"},
    {"English", "338"},
    {"English", "344"},
    {"English", "350"},
    {"English", "374"},
    {"English", "544"},
    {"English", "702"},
    {"",""}, //end indicator, do not delete;
}; 
#endif//   LANGUAGE_CHN

#endif//_SETTLANGCODETBL_H
