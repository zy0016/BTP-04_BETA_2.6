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

#ifndef _LPC_
#define REG_CopyOut(dst, src,len)   memcpy(dst, src, len)
#else
extern int REG_CopyOut(void * dst, const void * src, int len);
#endif

#define REG_TYPE_DIR            1
#define REG_TYPE_VALUE          2
#define REG_TYPE_STRING         3
#define REG_TYPE_DATA           4

typedef struct reg_curr {
    int     type;
    int     mode;
    struct reg_entity * dir;
} * PREG_CURR;

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

struct reg_entity * Reg_Curr_FindDir (PREG_CURR pReg,
            const char * path, const char **ppName);
struct reg_entity * Reg_Curr_FindEntry (PREG_CURR pReg,
            struct reg_entity * dir, const char * name, int namelen);
struct reg_entity * Reg_Curr_Find (PREG_CURR pReg, const char * path);

struct reg_entity Reg_Curr_Root = { &Reg_Curr_Root, 0, {0}, REG_TYPE_DIR };

static int reg_keyname;

PREG_CURR Reg_Curr_Open (int mode)
{
    PREG_CURR pReg;

    pReg = (PREG_CURR)malloc (sizeof(*pReg));
    pReg->mode = mode;
    /* Set to root dir */
    pReg->dir = &Reg_Curr_Root;
    return (PREG_CURR)pReg;
}

void Reg_Curr_Close (PREG_CURR pReg)
{
    free (pReg);
}

int Reg_Curr_Chdir (PREG_CURR pReg, const char * path)
{
    struct reg_entity * pDir;

    pDir = Reg_Curr_Find (pReg, path);
    if ( pDir == NULL || ! pDir->type == REG_TYPE_DIR ) {
        return 0;
    }
    pReg->dir = pDir;
    return 1;
}

struct reg_entity * Reg_Curr_FindEntry (PREG_CURR pReg,
            struct reg_entity * dir, const char * name, int namelen)
{
    struct reg_entity * p;

    if ( namelen == 1 && name[0] == '.' )
        return dir;
    if ( namelen == 2 && name[0] == '.' && name[1] == '.' )
        return dir->parent;

    p = dir->value.dir;
    while ( p != NULL ) {
        if( strncmp(p->name, name, namelen) == 0 && p->name[namelen] == 0 )
            return p;
        p = p->next;
    }
    return NULL;
}

struct reg_entity * Reg_Curr_FindDir (PREG_CURR pReg,
            const char * path, const char **ppName)
{
    struct reg_entity * dir;
    const char * ptr;

    if ( *path == '/' ) {   /* Strat from root */
        dir = &Reg_Curr_Root;
        path ++;
    } else {
        dir = pReg->dir;
    }

    while (dir != NULL && (ptr = strchr (path, '/')) != NULL) {
        int len = ptr - path;
        dir = Reg_Curr_FindEntry (pReg, dir, path, len);
        if ( dir == NULL || dir->type != REG_TYPE_DIR ) {
            dir = NULL;
            break;
        }
        /* To next section */
        path = ptr + 1;
    }
    if ( ppName )
        *ppName = path;
    return dir;
}

struct reg_entity * Reg_Curr_Find (PREG_CURR pReg, const char * path)
{
    struct reg_entity * dir;
    const char * name;

    dir = Reg_Curr_FindDir (pReg, path, &name);
    return (dir == NULL) ? NULL : Reg_Curr_FindEntry(pReg, dir, name, strlen(name));
}

int Reg_Curr_List(PREG_CURR pReg, const char * path, int index, char * buf, int buflen)
{
    struct reg_entity * pDir, * pKey;
    int count, len, totallen;
    char * ptr;

    pDir = Reg_Curr_Find(pReg, path);
    if ( pDir == NULL || pDir->type != REG_TYPE_DIR ) {
        return 0;
    }
    pKey = pDir->value.dir;
    count = totallen = 0;
    ptr = buf;
    while (pKey != NULL) {
        if ( index > 0 ) {
            index --;
        } else {
            len = strlen(pKey->name) + 3;
            if ( len < buflen - totallen )
                break;
            *ptr = len;
            *(ptr+1) = pKey->type;
            memcpy (ptr+2, pKey->name, len - 2);
            ptr += len;
            count ++;
        }
        pKey = pKey->next;
    }
    return count;
}

static struct reg_entity * Reg_Curr_NewKey(struct reg_entity * pDir,
                        int type, const char * name, int flag)
{
    struct reg_entity * pNew;
    struct reg_entity * p, **pp;

    /* Allocate one dir entry */
    pNew = malloc(sizeof(struct reg_entity) + strlen(name)+1);
    if ( pNew == NULL ) {
        return NULL;
    }
    strcpy (pNew->name, name);
    pNew->flags = flag;
    pNew->type = type;
    pNew->parent = pDir;
    pNew->value.data = 0;  /* Empty dir */
    /* Insert to end of list */
    pp = (struct reg_entity **)&pDir->value;
    while ( *pp ) {
        p = *pp;
        pp = &p->next;
    }
    pNew->next = 0;
    *pp = pNew;
    return pNew;
}

int Reg_Curr_MakeKey (PREG_CURR pReg, const char * path, int type)
{
    struct reg_entity * pDir;
    const char * name;

    pDir = Reg_Curr_FindDir(pReg, path, &name);
    if ( pDir == NULL ) {
        return 0;
    }

    if ( Reg_Curr_FindEntry (pReg, pDir, name, strlen(name)) != NULL ) {
        /* The dir exist */
        return 1;
    }

    return Reg_Curr_NewKey (pDir, type, name, pReg->type) != NULL;
}

int Reg_Curr_MakeDir (PREG_CURR pReg, const char * path)
{
    return Reg_Curr_MakeKey (pReg, path, REG_TYPE_DIR);
}

int Reg_Curr_MakeUniqKey (PREG_CURR pReg, int type,
              const char * path, char * namebuf, int buflen)
{
    struct reg_entity * pDir;

    if ( buflen < 14 )
        return 0;

    if ( path == NULL )
        pDir = pReg->dir;
    else {
        pDir = Reg_Curr_Find(pReg, path);
        if ( pDir == NULL || pDir->type != REG_TYPE_DIR ) {
            return 0;
        }
    }

    do {
        sprintf (namebuf, "UK%08X", reg_keyname ++);
    } while (Reg_Curr_FindEntry(pReg, pDir, namebuf, strlen(namebuf)) != NULL);

    return Reg_Curr_NewKey (pDir, type, namebuf, pReg->type) != NULL;
}

static int Reg_Curr_RmKey (PREG_CURR pReg, const char * path, int isdir)
{
    struct reg_entity * pKey, * p, ** pp;

    pKey = Reg_Curr_Find (pReg, path);
    if ( pKey == NULL ) /* The key not exist */
        return 0;
    if ( isdir ) {
        if ( pKey->type != REG_TYPE_DIR ||  /* Not dir */
                pKey->value.dir != NULL /* Dir not empty */ )
            return 0;
    } else {
        if ( pKey->type == REG_TYPE_DIR )
            return 0;
    }

    /* If this is a pointer key, remove the value */
    if ( pKey->type == REG_TYPE_STRING || pKey->type == REG_TYPE_DATA ) {
        if ( pKey->value.string != 0 ) {
            free (pKey->value.string);
            pKey->value.data = 0;
        }
    }
    /* Unlink it from parent dir list */
    p = pKey->parent;
    pp = (struct reg_entity **)&p->value;
    while ( *pp ) {
        p = *pp;
        if ( p == pKey ) {
            *pp = p->next;
            break;
        } else {
            pp = &p->next;
        }
    }
    /* Free the memory */
    free (pKey);
    return 1;
}

int Reg_Curr_DeleteKey (PREG_CURR pReg, const char * path)
{
    return Reg_Curr_RmKey (pReg, path, 0);
}

int Reg_Curr_RemoveDir (PREG_CURR pReg, const char * path)
{
    return Reg_Curr_RmKey (pReg, path, 1);
}

int Reg_Curr_SetKey (PREG_CURR pReg, const char * path, void * pData, int datalen)
{
    struct reg_entity * pKey;
    int len;

    pKey = Reg_Curr_Find (pReg, path);
    if ( pKey == NULL || (pKey->type != REG_TYPE_STRING && pKey->type != REG_TYPE_DATA) ) {
        return 0;
    }


    /* Free the value */
    if ( pKey->value.string != NULL ) {
        free (pKey->value.string);
        pKey->value.string = NULL;
    }

    if ( pData != NULL ) {
        if ( pKey->type == REG_TYPE_STRING ) {
            len = strlen ((char *)pData) + 1;
            pKey->value.string = malloc (len);
            if ( pKey->value.string == NULL )
                return 0;
            memcpy (pKey->value.string, pData, len);
        } else {
            pKey->value.p_data = malloc (sizeof(struct reg_value) + datalen);
            if ( pKey->value.p_data == NULL )
                return 0;
            pKey->value.p_data->len = datalen;
            memcpy (pKey->value.p_data->data, pData, datalen);
        }
    }
    return 1;
}

int Reg_Curr_GetValue (PREG_CURR pReg, const char * path, long * pValue)
{
    struct reg_entity * pKey;

    pKey = Reg_Curr_Find (pReg, path);
    if ( pKey == NULL || pKey->type != REG_TYPE_VALUE ) {
        return 0;
    }
    if ( pValue )
        *pValue = pKey->value.data;
    return 1;
}

int Reg_Curr_SetValue (PREG_CURR pReg, const char * path, long value)
{
    struct reg_entity * pKey;

    pKey = Reg_Curr_Find (pReg, path);
    if ( pKey == NULL || pKey->type != REG_TYPE_VALUE ) {
        return 0;
    }
    pKey->value.data = value;
    return 1;
}

int Reg_Curr_GetKey (PREG_CURR pReg, const char * path, void * pBuf, int buflen)
{
    struct reg_entity * pKey;
    int len;

    pKey = Reg_Curr_Find (pReg, path);
    if ( pKey == NULL ||
            (pKey->type != REG_TYPE_STRING && pKey->type != REG_TYPE_DATA) ) {
        return -1;
    }

    if ( pKey->value.data == 0 ) {
        return 0;
    }

    if ( pKey->type == REG_TYPE_STRING ) {
        len = strlen (pKey->value.string);
        if ( pBuf && len < buflen )
            strcpy (pBuf, pKey->value.string);
    } else {
        len = pKey->value.p_data->len;
        if ( pBuf && len <= buflen )
            memcpy (pBuf, pKey->value.p_data->data, len);
    }
    return len;
}
