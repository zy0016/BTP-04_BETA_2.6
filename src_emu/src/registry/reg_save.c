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


//#include "hmosal.h"
#include "plx_pdaex.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"

#include "registry.h"

#ifndef     _HOPEN
#define     REG_FILE        "hmap.reg"
#define     REG_FILE_TEMP   "switch.reg"
#define     REG_FILE_ALL    "regall.reg"
#else       
#define     REG_FILE        "/mnt/flash/hmap.reg"
#define     REG_FILE_TEMP   "/mnt/flash/switch.reg"
#define     REG_FILE_ALL    "/mnt/flash/regall.reg"
#endif

extern char reg_curr_file[];

struct reg_entity {
    struct reg_entity * parent; // Link to parent.
    struct reg_entity * next;   // Link to next.
    union {
        unsigned long data;     // Value of this key.
        void * string;          // Pointer to value.
        struct reg_entity * dir;    // Point to dir list.
        struct reg_value {
            unsigned short len;
            unsigned char  data[0];
        } * p_data;
    } value;
    unsigned char	type;
    unsigned char	flags;
    char            name[0];
};

extern struct reg_entity Reg_Curr_Root;
int Reg_DFS(struct reg_entity *pDir, FILE *fp);  // 
int Reg_DFS2(struct reg_entity *pDir, FILE *fp);
static int Reg_UpdateFile(const char* oldname, const char *newname);
/*********************************************************************\
* Function	   
* Purpose    �� REG_SAVE ����ע���洢���ļ�
* Params	   
* Return	 	   
* Remarks	
**********************************************************************/
int Reg_Save()
{
    FILE *fp;
    int ret;
    struct reg_entity *pDir, *pDirRoot; 

    fp = fopen(REG_FILE_TEMP, "wb");  // open temporary file
    if(fp == NULL)
        return 0;

    pDirRoot = pDir = Reg_Curr_Root.value.dir;

    Reg_DFS(pDir, fp);

    fclose(fp);

//    Reg_UpdateFile( REG_FILE_TEMP, REG_FILE ); // rename temporary file to destination file
    ret = Reg_UpdateFile( REG_FILE_TEMP, reg_curr_file ); // ����ʱ�ļ���ΪĿ���ļ�

    return ret;
}
/*********************************************************************
* Function	   
* Purpose   depth first search
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
int Reg_DFS(struct reg_entity *pDir, FILE *fp)
{
    char buf[64];

    if(pDir->flags == REG_CURR)
        return 0;

    switch(pDir->type)
    {
    case REG_TYPE_DIR:
        sprintf(buf, "<DIR %s>\r\n", pDir->name);
        fputs(buf, fp);
        if(pDir->value.dir != NULL)
        {
            struct reg_entity *pParent;
            pParent = pDir;
            Reg_DFS(pDir->value.dir, fp);
            strcpy(buf, "</DIR>\r\n");
            fputs(buf, fp);
            if(pParent->next != NULL)
                Reg_DFS(pParent->next, fp);
        }
        else if(pDir->next != NULL)
        {
            Reg_DFS(pDir->next, fp);
            strcpy(buf, "</DIR>\r\n");
            fputs(buf, fp);
        }
        else if(pDir->parent == &Reg_Curr_Root && pDir->next != NULL)
        {
            Reg_DFS(pDir->next, fp);
            strcpy(buf, "</DIR>\r\n");
            fputs(buf, fp);
        }
        else
        {
            strcpy(buf, "</DIR>\r\n");
            fputs(buf, fp);
        }
        break;

    case REG_TYPE_VALUE:
        sprintf(buf, "%s = %d\r\n", pDir->name, pDir->value.data);
        fputs(buf, fp);
        if(pDir->next != NULL)
            Reg_DFS(pDir->next, fp);
        break;

    case REG_TYPE_STRING:
        sprintf(buf, "%s = \"%s\"\r\n", pDir->name, (const char *)pDir->value.string);
        fputs(buf, fp);
        if(pDir->next != NULL)
            Reg_DFS(pDir->next, fp);
        break;

    case REG_TYPE_DATA:
        break;

    default:
        return 0;
    }

    return 1;
}

/*********************************************************************
* Function	
* Purpose   ���������ļ�������nPB_DbFileSize
*       	�����ļ�ʱ��������ʱ�ļ��������ļ�ʱ����ԭ�ļ�ɾ�����ٽ���ʱ
*           �ļ��ĵ�ԭ���ļ���
* Params    ��ʱ�ļ������ļ���С
* Return	1 	   
* Remarks	   
**********************************************************************/
static int Reg_UpdateFile(const char* oldname, const char *newname)
{
//	if(remove(newname) != 0)  // ��ԭ�ļ�ɾ��
//		return 0;

    remove(newname);
    //int rename( const char *oldname, const char *newname );
	if(rename(oldname, newname) != 0)  // ���м��ļ���Ϊԭ�ļ���
		return 0;

	return 1;
}
/*********************************************************************\
* Function	   
* Purpose    ��ע���ȫ���洢, ������ REG_CURR ���ͺ� REG_SAVE ����  
* Params	   
* Return	 	   
* Remarks	 for testing
**********************************************************************/
int Reg_Save2()
{
    FILE *fp;
    struct reg_entity *pDir;
    char *filename = "reg2.txt";

    fp = fopen(filename, "wb");
    if(fp == NULL)
        return 0;

    pDir = Reg_Curr_Root.value.dir;

    Reg_DFS2(pDir, fp);

    fclose(fp);

    return 1;
}
/*********************************************************************
* Function	   
* Purpose    for testing
**********************************************************************/
int Reg_DFS2(struct reg_entity *pDir, FILE *fp)  
{
    char buf[64];

    switch(pDir->type)
    {
    case REG_TYPE_DIR:
        sprintf(buf, "<DIR %s>\r\n", pDir->name);
        fputs(buf, fp);
        if(pDir->value.dir != NULL)
        {
            struct reg_entity *pParent;
            pParent = pDir;
            Reg_DFS2(pDir->value.dir, fp);
            strcpy(buf, "</DIR>\r\n");
            fputs(buf, fp);
            if(pParent->next != NULL)
                Reg_DFS2(pParent->next, fp);
        }
        else if(pDir->next != NULL)
        {
            Reg_DFS2(pDir->next, fp);
            strcpy(buf, "</DIR>\r\n");
            fputs(buf, fp);
        }
        else if(pDir->parent == &Reg_Curr_Root && pDir->next != NULL)
        {
            Reg_DFS2(pDir->next, fp);
            strcpy(buf, "</DIR>\r\n");
            fputs(buf, fp);
        }
        else
        {
            strcpy(buf, "</DIR>\r\n");
            fputs(buf, fp);
        }
        break;

    case REG_TYPE_VALUE:
        sprintf(buf, "%s = %d\r\n", pDir->name, pDir->value.data);
        fputs(buf, fp);
        if(pDir->next != NULL)
            Reg_DFS2(pDir->next, fp);
        break;

    case REG_TYPE_STRING:
        sprintf(buf, "%s = \"%s\"\r\n", pDir->name, (const char *)pDir->value.string);
        fputs(buf, fp);
        if(pDir->next != NULL)
            Reg_DFS2(pDir->next, fp);
        break;

    case REG_TYPE_DATA:
        break;

    default:
        return 0;
    }

    return 1;
}
