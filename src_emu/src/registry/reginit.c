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

//#include    "hmosal.h"
#include "plx_pdaex.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "ctype.h"

#include    "registry.h"

/*********************************
<DIR dirname>
    keyname = keyvalue
</DIR>
**********************************/
#define REG_FILE_CURR  "/mnt/flash/hmap.reg"
#define REG_FILE_SAVE  "/rom/hmap.reg"

#ifndef TRUE
#define TRUE	1
#define FALSE	0
#endif

static char * SkipSpace(char * p);
static int HandleToken(PREG pReg, char * line);
static int HandleKey(PREG pReg, char * line);

char reg_curr_file[128];
/*********************************************************************\
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks : original file	   
**********************************************************************/
#if 0
int reginit (const char * file)
{
    FILE * fp;
    PREG   pReg;
    char   buf[400];

    fp = fopen (file, "r");
    if ( fp == NULL ) 
        return FALSE;

//    pReg = Reg_Open (REG_CURR, 0);
    pReg = Reg_Open (REG_SAVE, 0);

    while ( ! feof (fp) ) {
        char * p = fgets (buf, sizeof(buf), fp);
        if ( p ) {
            p = SkipSpace (p);
            if ( *p && *p != '\n' ) {
                if ( *p == '<' ) {
                    if ( ! HandleToken(pReg, p) )
                        return FALSE;
                } else {
                    if ( ! HandleKey(pReg, p) )
                        return FALSE;
                }
            }
        }
    }
//    Reg_Close (pReg);
//    Reg_Curr_Close(pReg);
    free(pReg);
    fclose(fp);
    return TRUE;
}
#endif
/*********************************************************************\
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks : by yangyoubing	   
**********************************************************************/
static int CreateReg(PREG  pReg, FILE *fp); // declare
extern int Reg_Save();

int reginit (const char * file)
{
    FILE * fp;
    PREG   pReg;
    int bSave = 0;
    int bRet = FALSE;

    if ( file == NULL ) {
        fp = fopen(REG_FILE_CURR, "r");
        if ( fp == NULL ) {
            fp = fopen(REG_FILE_SAVE, "r");
            if ( fp == NULL )
                return FALSE;
            bSave = 1;
        }
        strcpy(reg_curr_file, REG_FILE_CURR);
    }else{
        fp = fopen(file, "r");
        if ( fp == NULL ) {
            fp = fopen(REG_FILE_SAVE, "r");
            if ( fp == NULL )
                return FALSE;
            bSave = 1;
        }
        strcpy(reg_curr_file, file);
    }


    pReg = Reg_Open (REG_SAVE, 0);
    bRet = CreateReg(pReg, fp);

    fclose(fp);
    free(pReg); // free memory that already be created

    if ( bRet == TRUE ) {
        if ( bSave ) 
            return Reg_Save();
        return TRUE;
    }else{
        if ( bSave )
            return FALSE;

        fp = fopen(REG_FILE_SAVE, "r");
        if ( fp == NULL )
            return FALSE;

        pReg = Reg_Open (REG_SAVE, 0); // open again
        bRet = CreateReg(pReg, fp);
        fclose(fp);
        free(pReg);
        if ( bRet == FALSE )
            return FALSE;

        return Reg_Save();
    }

    return TRUE;
}


/*********************************************************************\
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static int CreateReg(PREG  pReg, FILE *fp)
{
    char   buf[400];
    while ( ! feof (fp) ) {
        char * p = fgets (buf, sizeof(buf), fp);
        if ( p ) {
            p = SkipSpace (p);
            if ( *p && *p != '\n' ) {
                if ( *p == '<' ) {
                    if ( ! HandleToken(pReg, p) )
                        return FALSE;
                } else {
                    if ( ! HandleKey(pReg, p) )
                        return FALSE;
                }
            }
        }
    }
    return TRUE;
}

/*********************************************************************
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
char * SkipSpace(char * p)
{
    while ( isspace (*p) )
        p++;
    return p;
}

int HandleToken(PREG pReg, char * p)
{
    if ( strncmp (p, "</DIR>", 6) == 0 ) {
        Reg_Chdir (pReg, "..");
        return TRUE;
    }

    if ( strncmp (p, "<DIR", 4) == 0 && isspace(*(p+4)) ) {
        char * name = SkipSpace(p+5);
        p = strchr (name, '>');
        if ( p == NULL )
            return FALSE;
        /* delete the end space */
        do {
            *p -- = 0;
        } while (p > name && isspace(*p));
        if ( strlen(name) == 0 )
            return FALSE;
        /* make this dir */
        if ( Reg_MakeDir (pReg, name) && Reg_Chdir (pReg, name) )
            return TRUE;
        return TRUE;
    }
    return FALSE;
}

int HandleKey(PREG pReg, char * p)
{
    char * name = p;
    char * pt;

    p = strchr(name, '=');
    if ( p == NULL )
        return FALSE;

    /* delete the end space */
    pt = p;
    do {
        *pt -- = 0;
    } while (pt > name && isspace(*pt));
    if ( strlen(name) == 0 )
        return FALSE;

    /* Check value */
    p = SkipSpace (p+1);
    if ( *p == '"' ) {
        pt = strchr(p+1, '"');
        if ( pt == NULL )
            return FALSE;
        p = p+1;
        *pt = 0;
        if ( Reg_MakeKey (pReg, name, REG_TYPE_STRING) &&
                Reg_SetKey (pReg, name, p, 0) )
            return TRUE;
    } else {
        unsigned long value = strtol (p, NULL, 0);
        if ( Reg_MakeKey (pReg, name, REG_TYPE_VALUE) &&
                Reg_SetValue (pReg, name, value) )
            return TRUE;
    }
    return FALSE;
}
