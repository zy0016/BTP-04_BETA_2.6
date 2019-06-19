/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : 
 *
 * Purpose  : smil1.0 parser      
 *            
\**************************************************************************/

#include <stdio.h>
#include "ctype.h"
#include "smilparse.h"
#include "MmsEdit.h"
//#include "fapi.h"
#include "sys/types.h"
#include "sys/stat.h"
#include "fcntl.h"
#include "unistd.h"


#define  MAX_GSM_NUM    41

struct TagTypeSyntax
{
    int type;
    char name[MAX_NAME_LEN];
    unsigned char namelen;
    unsigned int size;
    unsigned char bEmptyTag;
};
struct tagNamedCharEntities 
{
    char *name;
    int  namelen;
    char value;
};
#define AMPESC_NUM  7

struct tagNamedCharEntities smilAmpEscapes[AMPESC_NUM] = 
{
    {"quot", 4, '\"'},
    {"amp",  3, '&'},
    {"apos", 4, '\''},
    {"lt",   2, '<'},
    {"gt",   2, '>'},
    {"nbsp", 4, (char)160},
    {"shy",  3, (char)173}
};


struct TagTypeSyntax  smilTagTable[MAX_TAG_NUM] =
{
    {TT_SMIL,           MT_SMIL,        4,  0,  0},
    {TT_BODY,           MT_BODY,        4,  0,  0},
    {TT_HEAD,           MT_HEAD,        4,  0,  0},
    {TT_LAYOUT,         MT_LAYOUT,      6,  0,  0},
    {TT_META,           MT_META,        4,  sizeof(TAG_META),   1},
    {TT_REGION,         MT_REGION,      6,  sizeof(TAG_REGION), 1},
    {TT_ROOT_LAYOUT,    MT_ROOT_LAYOUT, 11, sizeof(TAG_ROOTLAYOUT), 1},
    {TT_AUDIO,          MT_AUDIO,       5,  sizeof(TAG_AUDIO),  0},
    {TT_IMG,            MT_IMG,         3,  sizeof(TAG_IMG),    0},
    {TT_PAR,            MT_PAR,         3,  sizeof(TAG_PAR),    0},
    {TT_REF,            MT_REF,         3,  sizeof(TAG_IMG),    0},
    {TT_TEXT,           MT_TEXT,        4,  sizeof(TAG_IMG),    0},
};
static char *charset[] = 
{
    "big5",
    "iso-10646-ucs-2",
    "iso-8859-1",
    "iso-8859-2",
    "iso-8859-3",
    "iso-8859-4",
    "iso-8859-5",
    "iso-8859-6",
    "iso-8859-7",
    "iso-8859-8",
    "iso-8859-9",
    "shift_JIS",
    "us-ascii",
    "utf-8"
};


static WORD GsmCode[MAX_GSM_NUM] = 
{ 
  0,   1,  2,  3,  4,  5,  6,  7, 
  17,  9, 11, 12, 14, 15, 16, 18, 
  19, 20, 21, 22, 23, 24, 25, 26, 
  28, 29, 30, 31, 0x80, 0x81, 0x82, 0x83, 
  0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8a, 0x8b, 
  0x8c
};

static WORD UniCode[MAX_GSM_NUM] = 
{ 
  0X0040, 0X00A3, 0X0024, 0X00A5, 0X00E8, 0X00E9, 0X00F9, 0X00EC, 
  0X00F2, 0X00C7, 0X00D8, 0X00F8, 0X00C5, 0X00E5, 0X0394, 0X03A6, 
  0X0393, 0X039B, 0X03A9, 0X03A0, 0X03A8, 0X03A3, 0X0398, 0X039E, 
  0X00C6, 0X00E6, 0X00DF, 0X00C9, 0X00A4, 0X00A1, 0X00C4, 0X00D5, 
  0X00D1, 0X00DC, 0X00A7, 0X00BF, 0X00E4, 0X00F6, 0X00F1, 0X00FC, 
  0X00E0
};

static charsetnum = 14;
static int HexStrToInt(char * s);
static char * SkipPrologue(char *start);
static void  * MakeTag(TAG_SMIL *outSmil, char * start, char ** end);
static int ParseTagType(char ** str);
static void GetAttrValue(char * text, AttributeList * AttrList);
static BOOL MMS_TransferImgSize(SIZE* pSize);
void PARSER_FreePARSETRELT(PARSETRELT *pRelt);
#ifdef WB_FOR_MSWIN
void PrintParseResult(SMILTAG * SMILTag);
#endif

/******************************************************************
* Main parser of mms text.  Takes raw text, and produces a linked
* tree of tag objects.  Mark objects are either text strings, or
* starting and ending tag delimiters.
******************************************************************/
TAG_SMIL * SMILParse(TAG_SMIL *outSmil,char * buf,int charset)
{
    char * start, * end;
    //  char * text;
    int PreBlank;
    
    if (!outSmil ||!buf)
        return NULL;
    
    start = buf;
    
    memset(outSmil,0,sizeof(TAG_SMIL));
    /* skip over the prologue--content before <wml> */
    end = SkipPrologue(start);
    
    if (end == NULL) /* invalid prologue */
        return NULL;
    
    start = end; /* both start and end point to '<' of <wml> */
    
    MakeTag(outSmil, start, &end);
    
    start = end + 1;
    
    /* generate children */
    while(*start)   /* GetTag and GetText in turn */
    {
        PreBlank = 0;
        
        if (ISBLANK(*start))
        {
            PreBlank = 1;
            /* Skip over blank space */
            while(ISBLANK(*start))
                start++;
        }
        
        if (*start == '<') /* Get the next tag */
        {
            MakeTag(outSmil, start, &end);
            
            start = end + 1;
        }
        else
        {
            while ((*start != '\0') && (*start != '<'))         
                start++;        
        }

    }/* end while */
#ifdef WB_FOR_MSWIN
    //PrintParseResult(root);
#endif
    return outSmil;
}
/*
 * this function calculate the widechar number in widechar string.
 * remember, this widechar string must be terminated with 0x0.
 */
static int w_wcslen( const unsigned short *string )
{
    int count = 0;
    while (string[count] != 0x0) 
        count++;
    return count;
}

/* skip the content before <wml> tag.
return: the new start postion
*/
static char * SkipPrologue(char *start)
{
    /* skip over blankspaces */
    while(ISBLANK(*start))
        start++;
    
    if (*start != '<')
        return NULL;
    
    /* skip over the prologue of deck */
    if (strncmp(start, "<?xml", 5) == 0)
    {
        start++;
        while(*start != '<' && *start != 0)
            start++;
        if (*start && (strncmp(start, "<!DOCTYPE", 9) == 0))
        {
            start++;
            while(*start != '<' && *start)
                start++;
        }
    }
    if (*start == 0)
        return NULL;
    
    /* skip comment <!-- -->, if any.*/
    /* maybe there are more than one comment */
    while (strncmp (start, "<!--", 4) == 0)
    {
        /* skip over the comment text */
        /* end tag is --*>, where * is zero or more spaces  */
        while (*start != '\0')
        {
            if (strncmp (start, "--", 2) == 0)   /* found double dash (--) */
            {
                start += 2;
                while ((*start != '\0') && ((*start == ' ') || (*start == '\n')
                    || (*start == '-') ))
                    start++;                   /* skip spaces and newlines */ 
                if (*start == '>')             /* completed end comment */
                {
                    while (*start != '<' && *start) /* move to the next '<'*/
                        start++;
                    break;
                }
            }
            else     /* if no double dash (--) found */
                start++;
        }
        if (*start == '\0')
            return NULL;
    } /* end while */
    
    /* problematic! maybe there are other type of '<' before <wml>. */
    /* but it will do now. */
    
    return start;
}

/*
* Clean up the white space in a string.
* Remove all leading and trailing whitespace, and turn all
* internal whitespace into single spaces separating words.
* The cleaning is done by rearranging the chars in the passed
* txt buffer.  The resultant string will probably be shorter,
* it can never get longer.
*/
static void CleanWhiteSpace(char *txt)
{
    char *ptr;
    char *start;
    
    start = txt;
    ptr = txt;
    
    /* Remove leading white space */
    while(ISBLANK(*ptr))
        ptr++;
    
    
    /* find a word, copying if we removed some space already */
    if (start == ptr) /* no leading blank space */
    {
        while ((!ISBLANK((int)*ptr)) && (*ptr != '\0'))
            ptr++;
        
        start = ptr;
    }
    else
    {
        *start++ = ' ';  /* reserve one leading black space */
        while ((!ISBLANK((int)*ptr)) && (*ptr != '\0'))
            *start++ = *ptr++;
    }
    
    while (*ptr != '\0')
    {
        /* Remove trailing whitespace.*/
        while (ISBLANK((int)*ptr))
            ptr++;
        
        *start++ = ' ';
        
        if (*ptr == '\0')
            break;
        
            /* If there are more words, insert a space and if space was 
        removed move up remaining text.  */
        if (start == ptr)
        {
            while ((!ISBLANK((int)*ptr)) && (*ptr != '\0'))
                ptr++;
            
            start = ptr;
        }
        else
        {
            while ((!ISBLANK((int)*ptr)) && (*ptr != '\0'))
                *start++ = *ptr++;
        }
    }
    *start = '\0';
}

/*
* Clean the special WML character escapes out of the text and replace
* them with the appropriate characters "&lt;" = "<", "&gt;" = ">",
* "&amp;" = "&"
* GAG:  apperantly &lt etc. can be left unterminated, what a nightmare.
* Ok, better, they have to be terminated with white-space or ';'.
* the '&' character must be immediately followed by a letter to be
* a valid escape sequence.  Other &'s are left alone.
* The cleaning is done by rearranging chars in the passed txt buffer.
* if any escapes are replaced, the string becomes shorter.
*/

#define ARR_LEN  512 *2

static void CleanText(char * txt)
{
    char *ptr;
    char *ptr2;
    char *start;
    unsigned short awUnic[ARR_LEN];
    int idx, ret;
    
    if (txt == NULL)
        return;
    
        /*
        * Quick scan to find escape sequences.
        * Escape is '&' followed by a letter or a hash tag.
        * return if there are none.
    */
    ptr = txt;
    while (*ptr != '\0')
    {
        if ((*ptr == '&') && ((ISLETTER((int)*(ptr + 1))) || (*(ptr + 1) == '#')))
        {
            break;
        }
        ptr++;
    }
    if (*ptr == '\0')
        return;
    
    /* Loop, replaceing escape sequences, and moving up remaining text.*/
    ptr2 = ptr;
    while (*ptr != '\0')
    {
        /* Extract the escape sequence from start to ptr */
        start = ptr + 1;
        while ((*ptr != ';') && (!ISBLANK((int)*ptr)) && (*ptr != '\0'))
        {
            ptr++;
        }
        *ptr = '\0';
        
        if (*start == '#') /* numberic character entity */
        {
            idx = 0;
            while ((idx < ARR_LEN - 1) && *start)
            {
                start++;
                if (*start == 'x' || *start == 'X') /* hex */
                {
                    awUnic[idx] = (unsigned short)HexStrToInt(start + 1);
                }
                else
                {
                    awUnic[idx] = (unsigned short)atoi(start);
                }
                idx++;
                
                ptr++;
                if ((*ptr == '&') &&  (*(ptr + 1) == '#'))
                {
                    start = ptr + 1;
                    while ((*ptr != ';') && (!ISBLANK((int)*ptr)) && (*ptr != '\0'))
                        ptr++;
                    *ptr = '\0';
                }
                else
                {
                    break; /* break out of while */
                }
            }
            awUnic[idx] = 0;
            idx++;
            
            ret = WideCharToMultiByte(0, 0, awUnic, idx, ptr2, idx * 2, NULL, NULL);
            if (ret > 0)
                ptr2 += ret;
            
        }
        else  /* named character entity */
        {
            char cval = 0;
            int i, len, len0 = strlen(start);
            for(i = 0; i < AMPESC_NUM; i++)
            {
                len = smilAmpEscapes[i].namelen;
                if ((len0 == len) && (!strncmp(start, smilAmpEscapes[i].name, len)))
                {
                    cval = smilAmpEscapes[i].value;
                    break;
                }
            }
            if (cval != 0)
            {
                *ptr2 = cval;
            }
            else /* invalid escape sequence. skip it. */
            {
                *ptr = ';';
                ptr = start - 1;
                *ptr2 = *ptr;
            }
            ptr2++;
            ptr++;
        }/* end named character entity processing */
        
        /* Copy remaining text until the next escape sequence is found. */
        while (*ptr != '\0')
        {
            if ((*ptr == '&') && ((ISLETTER((int)*(ptr + 1)))||(*(ptr + 1) == '#')))
            {
                break;
            }
            *ptr2++ = *ptr++;
        }
    }/* end while (*ptr) */
    *ptr2 = '\0';
}

static char* SetDefValue(char *szDefValue)
  {
      int vlen;
      char *buf;
      
      
      //    return szDefValue;
      
      if (!szDefValue) 
          return NULL;
      
      vlen = strlen(szDefValue);
      
      buf = MMS_malloc(vlen +1);
      
      if (!buf) 
          return NULL;
      
      strcpy(buf,szDefValue);
      
      return buf;
  }
  /*
  * Get a block of text from a HTML document.
  * All text from start to the end, or the first tag
  * (a tag is '<' or '</' followed by any letter or a '!')
  * is returned in a MMS_malloced buffer.  Also, endp returns
  * a pointer to the next '<' or '\0'
  * The returned text has already expanded '&' escapes.
  */
  //SMILTAG *psmil,
  static BOOL  NewTagObj(TAG_SMIL *pSmil,char * text)
  {
      char * str;
      int TagType;
      AttributeList AttrList;
      int size;
      char ** value;
      int ibEND;
      void *newtag;
      
      str = text;
      
      TagType = ParseTagType(&text);
      
      if (TagType == TT_UNKNOWN)
          return FALSE;
      
      size = smilTagTable[TagType - 1].size;
      if (size)
      {
          newtag = MMS_malloc(size);
          if (newtag == NULL)
              return FALSE;
          memset(newtag, 0, size);
      }
      
      if (smilTagTable[TagType - 1].bEmptyTag || (str[strlen(str) - 1] == '/'))
      {
          ibEND = TRUE;
      }
      else
      {
          ibEND = FALSE;
      }
      
      
      if (smilTagTable[TagType - 1].bEmptyTag || (str[strlen(str) - 1] == '/'))
      {
          ibEND = TRUE;
      }
      else
      {
          ibEND = FALSE;
      }
      
      /* parse tag's attributes */
      switch(TagType)
      {
      case TT_SMIL:
      case TT_HEAD:
      case TT_BODY:  
      case TT_LAYOUT:         
          break;
      case TT_REGION:       
          {
              TAG_REGION *tag = (TAG_REGION *)newtag;
              
              AttrList.count = 6;
              AttrList.name[0] = "left";
              AttrList.name[1] = "top";
              AttrList.name[2] = "height";
              AttrList.name[3] = "width";
              AttrList.name[4] = "fit";
              AttrList.name[5] = "id";
              
              GetAttrValue(text, &AttrList);
              value = AttrList.value;       
              
              if (!value[0]) 
                  value[0]  =   SetDefValue("0");
              if (!value[1]) 
                  value[1]  =   SetDefValue("0");
              
              tag->left = value[0];
              tag->top = value[1];
              tag->height = value[2];
              tag->width = value[3];
              tag->fit = value[4];
              tag->id = value[5];
              
              if ( !pSmil->regionh)
              {
                  pSmil->regionh = tag;
                  pSmil->regiont = tag;
              }
              else
              {
                  pSmil->regiont->pnext = tag;
                  pSmil->regiont = tag;
              }
              
          }
          break;
          
      case TT_ROOT_LAYOUT:
          {
              TAG_ROOTLAYOUT *tag = (TAG_ROOTLAYOUT *)newtag;
              
              AttrList.count = 2;
              AttrList.name[0] = "height";
              AttrList.name[1] = "width";
              
              GetAttrValue(text, &AttrList);
              value = AttrList.value;
              
              tag->height = value[0];
              tag->width = value[1];
              
              if ( !pSmil->rootlayouth)
              {
                  pSmil->rootlayouth = tag;
                  pSmil->rootlayoutt = tag;
              }
              else
              {
                  pSmil->rootlayoutt->pnext = tag;
                  pSmil->rootlayoutt = tag;
              }
          }
          break;
          
      case TT_META:
          {
              TAG_META *tag = (TAG_META *)newtag;
              AttrList.count = 2;
              AttrList.name[0] = "name";
              AttrList.name[1] = "content";
              
              GetAttrValue(text, &AttrList);
              
              value = AttrList.value;
              
              tag->name = value[0];
              tag->content = value[1];
              
              if ( !pSmil->metah)
              {
                  pSmil->metah=tag;
                  pSmil->metat = tag;
              }
              else
              {
                  pSmil->metat->pnext = tag;
                  pSmil->metat = tag;
              }
              
          }
          break;
          
      case TT_PAR:
          {
              TAG_PAR *tag = (TAG_PAR *)newtag;
              AttrList.count = 1;
              AttrList.name[0] = "dur";
              
              GetAttrValue(text, &AttrList);
              value = AttrList.value;       
              
              pSmil->nParNum++;   
              tag->dur = value[0];
              tag->pnext =NULL;
              
              if ( !pSmil->parh)
              {
                  pSmil->parh=tag;  
                  pSmil->part = tag;
              }
              else
              {
                  pSmil->part->pnext = tag;
                  pSmil->part = tag;
              }
              
          }
          break;
          
          
      case TT_TEXT:
      case TT_IMG:
      case TT_REF:
          {
              TAG_TEXT *tag = (TAG_TEXT *)newtag;
              
              AttrList.count = 5;
              AttrList.name[0] = "src";
              AttrList.name[1] = "region";
              AttrList.name[2] = "alt";
              AttrList.name[3] = "begin";
              AttrList.name[4] = "end";
              
              
              GetAttrValue(text, &AttrList);
              value = AttrList.value;
              
              tag->src = value[0];
              tag->region = value[1];
              tag->alt = value[2];
              tag->begin = value[3];
              tag->end      = value[4];
              
              if (TagType == TT_IMG)
                  pSmil->part->timg = tag;
              else if (TagType == TT_TEXT)
                  pSmil->part->ttxt = tag;
              else
                  pSmil->part->tref = tag;
              
          }
          break;
          
      case TT_AUDIO:
          {
              TAG_AUDIO* tag = (TAG_AUDIO *)newtag;
              
              AttrList.count = 4;
              AttrList.name[0] = "src";
              AttrList.name[1] = "alt";
              AttrList.name[2] = "begin";
              AttrList.name[3] = "end";
              
              
              GetAttrValue(text, &AttrList);
              value = AttrList.value;
              
              tag->src = value[0];
              tag->alt = value[1];
              tag->begin = value[2];
              tag->end = value[3];
              pSmil->part->taudio = tag;
          }
          break;
          
      default:
          break;
          }
          
          
          return ibEND;
}

/*
* Get the tag text between '<' and '>'.  From the text, determine
* its type, and fill in a SMILTAG structure to return.  Also returns
* endp pointing to the ttrailing '>' in the original string.
*/
static void *MakeTag(TAG_SMIL *pSmil, char * start, char ** endp)
{
    char *ptr;
    char *text;
    char tchar;
    
    if (start == NULL)
        return(NULL);
    
    ptr = start;
    /* skip over blank space */
    while(ISBLANK(*ptr))
        ptr++;
    
    if (*ptr != '<')
        return NULL;
    
    /* check if we are in a comment, start tag is <!-- */
    if (strncmp (ptr, "<!--", 4) == 0)
    {
        /* skip over the comment text */
        /* end tag is --*>, where * is zero or more spaces  */
        while (*ptr != '\0')
        {
            if (strncmp (ptr, "--", 2) == 0)   /* found double dash (--) */
            {
                ptr += 2;
                while ((*ptr != '\0') && ((*ptr == ' ') || (*ptr == '\n')
                    || (*ptr == '-') ))
                    ptr++;                   /* skip spaces and newlines */ 
                if (*ptr == '>')             /* completed end comment */
                {
                    *endp = ptr;
                    return NULL;
                }
            }
            else     /* if no double dash (--) found */
                ptr++;
        }
        *endp = ptr;
        return NULL;  /* comment is not completed */
    } /* end if comment */
    
    ptr++;
    
    /* skip over white space */
    while(ISBLANK(*ptr))
        ptr++;
    
    start = ptr;
    
    while ((*ptr != '>') && (*ptr != '\0'))
        ptr++;
    
    if (*ptr != '>')
        return(NULL);
    
    *endp = ptr;
    
    tchar = *ptr;
    *ptr = '\0';
    
    if (*start == '/')
    {
        int TagType;
        char * tptr = start + 1;
        TagType = ParseTagType(&tptr);
        *ptr = tchar;
    }
    else
    {
        text = (char *)MMS_malloc(strlen(start) + 1);
        if (text == NULL)
        {
            *ptr = tchar;
            return(NULL);
        }
        strcpy(text, start);
        *ptr = tchar;
        
        CleanText(text); /* clean it of escapes, and odd white space. */
        NewTagObj(pSmil,text);
        MMS_free(text);
    }
    return NULL;
}

/* Determine tag type from the identifying string passed */
static int ParseTagType(char ** str)
{
    int len, i, j;
    char * tptr, * s;
    char tchar;
    char tchar2 ='\0';
    
    if (str == NULL || *str == NULL)
        return(TT_NONE);
    
    tptr = *str;
    while(ISBLANK(*tptr))
        tptr++;
    s = tptr;
    
    while (*tptr != '\0')
    {
        if (isspace((int)*tptr))
            break;
        
        tptr++;
    }
    tchar = *tptr;
    *tptr = '\0';
    
    len = strlen(s);
    
    if (s[len - 1] == '/')
    {
        len--;
        tchar2 = '/';
        s[len] = '\0';
    }
    
    for(i = 0; i < MAX_TAG_NUM; i++)
    {
        if (len ==smilTagTable[i].namelen)
        {
            j = 0;
            while((s[j] == smilTagTable[i].name[j]) && (j < len))
                j++;
            
            if (j == len)
            {
                *tptr = tchar;
                *str = tptr;
                if (tchar2 == '/')
                    s[len] = '/';
                return smilTagTable[i].type;
            }
        }
    }
    if (tchar2 == '/')
        s[len] = '/';
    *tptr = tchar;
    *str = tptr;
    return TT_UNKNOWN;
}

/*
* Parse a single attr.  ptrp is a pointer to a pointer to the string to be parsed.  
* On return, the ptr should be changed to point to after the text we have parsed.
* On return start and end should point to the beginning, and just
* after the end of the tag's name in the original anchor string.
* Finally the function returns the attr value in a MMS_malloced buffer.
*/
static char * ScanAttr(char ** ptrp, char ** startp, char ** endp)
{
    char *ptr;
    char *start;
    char tchar;
    int quoted;
    int has_value;
    char * attr_val;
    
    quoted = 0;
    
    /* remove leading spaces, and set start*/
    ptr = *ptrp;
    while (ISBLANK((int)*ptr))
    {
        ptr++;
    }
    *startp = ptr;
    
    /* Find and set the end of the attr */
    while ((!ISBLANK((int)*ptr)) && (*ptr != '=') && (*ptr != '\0'))
    {
        ptr++;
    }
    *endp = ptr;
    
    has_value=0;
    if (*ptr == '\0')
    {
        *ptrp = ptr;
    } 
    else /* Move to the start of the attr value, if there is one. */
    {
        while ((ISBLANK((int)*ptr)) || (*ptr == '='))
        {
            if (*ptr == '=')
            {
                has_value = 1;
            }
            ptr++;
        }
    }
    /* For a tag with no value, this is a boolean flag.
       Return the string "1" so we know the tag is there. */
    if (!has_value)
    {
        *ptrp = *endp;
#if 0        
        
        /* set a tag value of 1. */
        tag_val = (char *)MMS_malloc(strlen("1") + 1);
        if (tag_val == NULL)
        {
            return(NULL);
        }
        strcpy(tag_val, "1");
        return(tag_val);
#endif
        return NULL;
    }
    
    if (*ptr == '\"' )
    {
        quoted = 1;
        
        ptr++;
    }
    else if ( *ptr == '\'')
    {
        quoted = 2;
        ptr++;
    }
    
    start = ptr;
    /* Get tag value. Either a quoted string or a single word*/
    if (quoted == 1)
    {
        while ((*ptr != '\"')  && (*ptr != '\0'))
            ptr++;
    }
    else if (quoted == 2)
    {
        while ((*ptr != '\'') && (*ptr != '\0'))
            ptr++;
    }
    else
    {
        while ((!ISBLANK((int)*ptr)) && (*ptr != '\0'))
            ptr++;
    }
    
    /* Copy the tag value out into a MMS_malloced string*/
    tchar = *ptr;
    *ptr = '\0';
    attr_val = (char *)MMS_malloc(strlen(start) + 1);
    if (attr_val == NULL)
    {
        *ptr = tchar;
        *ptrp = ptr;
        
        return NULL;
    }
    strcpy(attr_val, start);
    *ptr = tchar;
    
    /* If you forgot the end quote, you need to make sure you aren't
    indexing ptr past the end of its own array */
    if (quoted && *ptr != '\0')
    {
        ptr++;
    }
    *ptrp = ptr;
    
    return attr_val;
}

/*
* Parse tag text for the value associated with the passed attr.
* If the passed attr is not found, return NULL.
* If the passed attr is found but has no value, return "".
*/
static void GetAttrValue(char * text, 
                         AttributeList * AttrList)
{
    char *ptr;
    char *start = NULL;
    char *end = NULL;
    char *attr_val;
    char tchar;
    int i, count;
    char ** NameList;
    char ** ValueList;
    
    if ((text == NULL) || (AttrList == NULL) || (AttrList->name == NULL))
        return;
    
    count = AttrList->count;
    NameList = AttrList->name;
    ValueList = AttrList->value;
    
    for(i = 0; i < count; i++)
        ValueList[i] = NULL;
    
    ptr = text;
    
    while (*ptr != '\0')
    {
        attr_val = ScanAttr(&ptr, &start, &end);
        
        tchar = *end;
        *end = '\0';
        
        for(i = 0; i < count; i++)
        {
            if (strcmp(start, NameList[i]) == 0)
            {
                ValueList[i] = attr_val;
                
                break; /* skip out of for */
            }
        }
        if (i == count) /* no matching attr name */
        {
            MMS_free(attr_val);
        }
        
        *end = tchar;
    }
}


/* ================================================= */
#ifdef WB_FOR_MSWIN
/* Print the contents of a parsed object tree, for debug */
void PrintTag(FILE * fp, SMILTAG * tag, int level)
{
    int i;
    
    if (tag == NULL)
        return;
    
    for(i = 0; i < level; i++)
        fprintf(fp, "\t");
    
    if (tag->TagType == TT_UNKNOWN)
        fprintf(fp, "unknown");
    else if (tag->TagType == TT_NONE)
        fprintf(fp, "TEXT");
    else
        fprintf(fp, TagTable[tag->TagType - 1].name);
    
    fprintf(fp, "\n");
    
    PrintTag(fp, tag->child, level + 1);
    
    PrintTag(fp, tag->sibling, level);
}

void PrintParseResult(SMILTAG * root)
{
    FILE * fp;
    
    fp = fopen("ParseRst.txt", "w");
    if (fp == NULL)
        return;
    
    PrintTag(fp, root, 0);
    
    fclose(fp);
}

#endif


static SYSTEMTIME My_GetDate( const char* SrcStr )
{
    SYSTEMTIME gDate;

    //add by lily
    GetLocalTime(&gDate);
    
    return gDate;
}


/*get the content of fixed field in head */
static char*  URL_GetHeadField( char* pUprStr, char* FieldName, unsigned long* pLen,
                               unsigned long *pOffset)
{
    char  tmpFieldName[129];
    char* pField = NULL, * pValue = NULL;
    char* pChar;
    
    strncpy(tmpFieldName , FieldName,128);
    tmpFieldName[128]='\0';
    
    if ((pField = strstr(pUprStr, tmpFieldName)) != NULL )
    {
        pValue = pField + strlen(tmpFieldName);
        
        while (*pValue == ' ') /* skip over blank space */
            pValue++;
        
        pChar = pValue;
        
        while (*pChar && *pChar != 0x0D && *pChar != 0x0A)
            pChar++;
        
        *pLen = pChar - pValue;
        *pOffset = pValue - pUprStr;
    }
    return pValue;
}

WSPHead *ParseRecvHead(char *pBuffer, int Count,WSPHead *pNewHead)
{
    char *token, *tokensrc; 
    int  nFieldNum, nFieldLen, i;
    char szField[SIZE_1];
    char *FieldStr[] = 
    {
        HHFN_FROM, HHFN_CONTENTTYPE, HHFN_TO, HHFN_CC, HHFN_BCC,
        HHFN_SUBJECT, HHFN_BREPORT, HHFN_BREPLY, HHFN_EXPIRY,
        HHFN_PRIORITY, HHFN_DATE, HHFN_CONTENTID, HHFN_CONTENTLOCATION
    };
    
    if (pBuffer == NULL && Count <= 0)
        return NULL;
    
    nFieldNum = 13;
    szField[0] = '\0';
    
    pBuffer[Count] = '\0';
    token = MMS_strtok(pBuffer, "\r\n");
    while (token != NULL) 
    {
        while(*token == 0x20)
            token++;
        nFieldLen = 0;        
        tokensrc = token;
        while (*token)
        {
            if (*token == ':')
            {
                strncpy(szField, tokensrc, nFieldLen + 1);
                szField[nFieldLen + 1] = '\0';                
                *token++;
                break;
            }
            *token++;
            nFieldLen++;
        }
        
        while(*token == 0x20)
            token++;
        for (i = 0; i < nFieldNum; i++)
        {
            char *ptmp;
            int cplen;
            if (!(*token))
                break;
            if (stricmp(szField, FieldStr[i]) == 0)
            {
                switch(i) 
                {
                case 0:         // HHFN_FROM                    
                    ptmp  = strstr(token, "/TYPE");
                    if ( ptmp != NULL)
                        cplen = ptmp - token;
                    else
                        cplen = strlen(token);

                    strncpy(pNewHead->from, token, cplen);
                    pNewHead->from[cplen] = '\0';
                    break;
                    
                case 1:         // HHFN_CONTENTTYPE
                    ptmp  = strchr(token, ';');
                    if ( ptmp != NULL)
                        cplen = ptmp - token;
                    else
                        cplen = strlen(token);

                    strncpy(pNewHead->ConType, token, cplen);
                    pNewHead->ConType[cplen] = '\0';
                    break;
                case 2:         // HHFN_TO
                    ptmp  = strstr(token, "/TYPE");
                    if ( ptmp != NULL)
                        cplen = ptmp - token;
                    else
                        cplen = strlen(token);

                    if(pNewHead->to[0] == '\0')
                    {
                        strncpy(pNewHead->to, token, cplen);
                        pNewHead->to[cplen] = '\0';
                    }
                    else
                    {
						int oldlen;

                        strcat(pNewHead->to, MMS_ASEPSTR);

						oldlen = strlen(pNewHead->to);

                        strncat(pNewHead->to, token, cplen);
                        pNewHead->to[cplen+oldlen] = 0;
                    }
                    break;
                case 3:         // HHFN_CC
                    ptmp  = strstr(token, "/TYPE");
                    if ( ptmp != NULL)
                        cplen = ptmp - token;
                    else
                        cplen = strlen(token);
                    strncpy(pNewHead->cc, token, cplen);
                    pNewHead->cc[cplen] = '\0';
                    break;
                case 4:         // HHFN_BCC,     
                    ptmp  = strstr(token, "/TYPE");
                    if ( ptmp != NULL)
                        cplen = ptmp - token;
                    else
                        cplen = strlen(token);

                    strncpy(pNewHead->Bcc, token, cplen);
                    pNewHead->Bcc[cplen] = '\0';
                    break;
                case 5:         // HHFN_SUBJECT, 
                    {
                        int len, sublen = 0;
                        char *p;
                        
                        p = strchr(token, ';');
                        if ( p == NULL) 
                        {
                            len = UTF8ToMultiByte(0, 0, token, strlen(token),
                                pNewHead->subject, 
                                SIZE_1, NULL, NULL);
                            pNewHead->subject[len] = '\0';
                        }
                        else
                        {                            
                            sublen = p - token;
                            len = UTF8ToMultiByte(0, 0, token, sublen, 
                                pNewHead->subject, 
                                SIZE_1, NULL, NULL);
                            pNewHead->subject[len] = '\0';
                        }
                    }
                    break;
                case 6:         // HHFN_BREPORT, 
                    if(strnicmp(token, "YES", 3) == 0)
                        pNewHead->bReport = 1;
                    else
                        pNewHead->bReport = 0;
                    break;
                case 7:         // HHFN_BREPLY, 
                    if(strnicmp(token, "YES", 3) == 0)
                        pNewHead->bReply = 1;
                    else
                        pNewHead->bReply = 0;
                    break;
                case 8:         // HHFN_EXPIRE,                                             
                    break;
                case 9:        // HHFN_PRIORITY, 
                    if(strnicmp(token, "HIGH", 4) == 0)
                        pNewHead->Priority = 0;
                    else if(strnicmp(token, "LOW", 3) == 0)
                        pNewHead->Priority = 1;
                    else
                        pNewHead->Priority = 2;
                    break;
                case 10:        // HHFN_DATE, 
                {
                    TIME_ZONE_INFORMATION TimeZoneInformation;
                    
                    MMS_StrToSysTime(token, &pNewHead->date);
                    GetTimeZoneInformation(&TimeZoneInformation);
                    SystemTimeToTzSpecificLocalTime(&TimeZoneInformation,
                        &pNewHead->date, &pNewHead->date);
                    break;
                }

                case 11:        // HHFN_CONTENTID,                    
                    break;
                case 12:        // HHFN_CONTENTLOCATION
                    break;
                default:
                    break;
                }//end switch
                break;
            }//end if
        }//end for
        token = MMS_strtok(NULL, "\r\n");   
    }//end while
    
    return pNewHead;    
}

/*********************************************************************\
* Function     MMS_ParseAccept
* Purpose      parse accept
* Params       pAcceptBuf: 
               bufLen:     
               pAcceptInfo:
* Return       void
**********************************************************************/
void MMS_ParseAccept(const char *pAcceptBuf, int bufLen, PACCEPTINFO pAcceptInfo)
{
    char *token, *tokensrc; 
    int  nFieldNum, nFieldLen, i;
    char szField[SIZE_1];
    char *pDupAccept;
    char *FieldStr[] = 
    {   
        HHFN_MSGTYPE, HHFN_TRANSID, HHFN_VERSION, HHFN_FROM, 
        HHFN_SUBJECT, HHFN_MSGCLASS, HHFN_MSGSIZE, HHFN_EXPIRY,
        HHFN_ACONTLOCA
    };
    
    if (pAcceptBuf == NULL && bufLen <= 0)
        return;
    
    nFieldNum = 9;
    szField[0] = '\0';
    pDupAccept = MMS_malloc(bufLen + 1);
    strncpy(pDupAccept, pAcceptBuf, bufLen);
    pDupAccept[bufLen] = 0;

    token = MMS_strtok(pDupAccept, "\r\n");
    while (token != NULL) 
    {
        while(*token == 0x20)
            token++;
        nFieldLen = 0;        
        tokensrc = token;
        while (*token)
        {
            if (*token == ':')
            {
                strncpy(szField, tokensrc, nFieldLen + 1);
                szField[nFieldLen + 1] = '\0';                
                *token++;
                break;
            }
            *token++;
            nFieldLen++;
        }
        
        while(*token == 0x20)
            token++;
        for (i = 0; i < nFieldNum; i++)
        {
            char *ptmp;
            int cplen;
            if (!(*token))
                break;
            if (stricmp(szField, FieldStr[i]) == 0)
            {
                switch(i) 
                {
                case 0:     // HHFN_MSGTYPE
                    pAcceptInfo->MsgType = atoi(token);
                    break;

                case 1:         // HHFN_TRANSID                    
                    strcpy(pAcceptInfo->TranId, token);
                    break;

                case 2:         // HHFN_VERSION
                    strcpy(pAcceptInfo->Version, token);
                    break;

                case 3:         // HHFN_FROM                    
                    ptmp  = strstr(token, "/TYPE");
                    if ( ptmp != NULL)
                        cplen = ptmp - token;
                    else
                        cplen = strlen(token);

                    strncpy(pAcceptInfo->From, token, cplen);
                    pAcceptInfo->From[cplen] = '\0';
                    break;               
                                
                case 4:         // HHFN_SUBJECT, 
                    {
                        int len, sublen = 0;
                        char *p;
                        
                        p = strchr(token, ';');
                        if ( p == NULL) 
                        {
                            len = UTF8ToMultiByte(0, 0, token, strlen(token), 
                                pAcceptInfo->Subject, 
                                SIZE_1, NULL, NULL);
                            pAcceptInfo->Subject[len] = '\0';
                        }
                        else
                        {                            
                            sublen = p - token;
                            len = UTF8ToMultiByte(0, 0, token, sublen, 
                                pAcceptInfo->Subject, 
                                SIZE_1, NULL, NULL);
                            pAcceptInfo->Subject[len] = '\0';
                        }
                    }
                    break;

                case 5:         // HHFN_MSGCLASS, 
                    strcpy(pAcceptInfo->MsgClass, token);
                    break;

                case 6:         // HHFN_MSGSIZE, 
                    pAcceptInfo->MsgSize = atoi(token);
                    //strcpy((char *)pAcceptInfo->MsgSize, token);
                    break;

                case 7:         // HHFN_AEXPIRY
                    strcpy(pAcceptInfo->ExpireTime, token);
                    break;

                case 8:        // HHFN_ACONTLOCA, 
                    strcpy(pAcceptInfo->ConLocation, token);
                    break;
                default:
                    break;
                }//end switch
                break;
            }//end if
        }//end for
        token = MMS_strtok(NULL, "\r\n");   
    }//end while

    MMS_free(pDupAccept);
    pDupAccept = NULL;
}
/*********************************************************************\
* Function     GetMetaType
* Purpose      
* Params       contTypeStr: the string include the ContentType
* Return        type
**********************************************************************/
static unsigned char GetMetaType(char *ContTypeStr)
{

    strupr(ContTypeStr);

    if (strstr(ContTypeStr, "AUDIO/MIDI") != NULL
        || strstr(ContTypeStr, "AUDIO/MID") != NULL
        || strstr(ContTypeStr, "AUDIO/X-SMF") != NULL
        || strstr(ContTypeStr, "AUDIO/SP-MIDI") != NULL
        || strstr(ContTypeStr, "AUDIO/SPMIDI") != NULL
        || strstr(ContTypeStr, "AUDIO/X-MIDI") != NULL
        || strstr(ContTypeStr, "AUDIO/X-MID") != NULL
        || strstr(ContTypeStr, "APPLICATION/X-MIDI") != NULL
        || strstr(ContTypeStr, "X-MUSIC/X-MIDI") != NULL
        || strstr(ContTypeStr, "MUSIC/CRESCENDO") != NULL)
        return META_AUDIO_MIDI;
    if (strstr(ContTypeStr, "AUDIO/AMR") != NULL 
        || strstr(ContTypeStr, "AUDIO/X-AMR") != NULL)
        return META_AUDIO_AMR;
    if (strstr(ContTypeStr, "AUDIO/WAV") != NULL
        || strstr(ContTypeStr, "AUDIO/X-WAV") != NULL)
        return META_AUDIO_WAV;  
    if (strstr(ContTypeStr, "APPLICATION/X-SMAF") != NULL
        || strstr(ContTypeStr, "AUDIO/MMF") != NULL
        || strstr(ContTypeStr, "APPLICATION/VND.SMAF") != NULL
        || strstr(ContTypeStr,"AUDIO/X-SMAF") != NULL
        || strstr(ContTypeStr, "APPLICATION/X-SMAF.MMF") != NULL)
        return META_AUDIO_MMF;
    if (strstr(ContTypeStr, "AUDIO/MP3") != NULL
        || strstr(ContTypeStr, "AUDIO/MPEG") != NULL)
        return META_AUDIO_MP3;
    if (strstr(ContTypeStr, "TEXT/X-IMELODY") != NULL
        || strstr(ContTypeStr, "AUDIO/IMELODY") != NULL)
        return META_AUDIO_IMY;
    if (strstr(ContTypeStr, "IMAGE/GIF") != NULL)
        return META_IMG_GIF;
    if (strstr(ContTypeStr, "IMAGE/JPEG") != NULL
        || strstr(ContTypeStr,"IMAGE/JPG") != NULL
        || strstr(ContTypeStr, "IMAGE/PJPEG") != NULL)
        return META_IMG_JPG;
    if (strstr(ContTypeStr, "IMAGE/BMP") != NULL
        || strstr(ContTypeStr, "IMAGE/X-BMP") != NULL
        || strstr(ContTypeStr, "IMAGE/BITMAP") != NULL)
        return META_IMG_BMP;
    if (strstr(ContTypeStr, "IMAGE/PNG") != NULL
        || strstr(ContTypeStr, "IMAGE/X-PNG") != NULL
        || strstr(ContTypeStr, "IMAGE/X-UP-WPNG") != NULL)
        return META_IMG_PNG;
    if (strstr(ContTypeStr, "IMAGE/VND.WAP.WBMP") != NULL)
        return META_IMG_WBMP;
    if (strstr(ContTypeStr, "TEXT/PLAIN") != NULL)
        return META_TEXT_PLAIN;
    if (strstr(ContTypeStr, "TEXT/X-VCALENDAR") != NULL)
        return META_TEXT_VCALE;
    if (strstr(ContTypeStr, "TEXT/X-VCARD") != NULL)
        return META_TEXT_VCARD;
    if (strstr(ContTypeStr, "SMIL") != NULL)
        return META_SMIL;
    return META_OTHER;
}
// trim the <> of contentid
static char * GetIdContent(char *pContentid, int idlen)
{
    char *pDupid, *cpdup;
    char *cp = pContentid;
    
   // ASSERT(pContentid != NULL || idlen > 0);//xlzhu

    pDupid = MMS_malloc(idlen + 1);
    strcpy(pDupid, pContentid);

    cpdup = pDupid;

    while (*cpdup)
    {
        if (*cpdup == 0x3C)    // 0x3c "<"
        {
            cpdup++;
            break;
        }
        cpdup++;
    }

    while(*cpdup)
    {
        if (*cpdup == 0x3E)    // 0x3e ">"
        {
            *cp = 0;
            break;
        }
        *cp++ = *cpdup++;
    }
    
    MMS_free(pDupid);
    return( pContentid );
}
//change the high byte and low byte
void HLTRANSPOS(unsigned short *unicode, int len)
{
    int i = 0;
    unsigned short tmp;
    unsigned char c1,c2;        

    for(i = 0; i<=len; i+=2)
    {
        c1 = (unsigned char)*unicode;
        c2 = (*unicode & 0xff00) >> 8;
        tmp = c1;
        *unicode = (tmp << 8) | c2;
        *unicode++;
    }
}

static unsigned char GetCharset(char *CharsetStr)
{
    char *pdest;
    char *pStr;
    int  len = 0;
    int  i;
    int  nType;
  
    nType = GetMetaType(CharsetStr);

    if (CharsetStr[0] == '\0')
        return -1;

    strupr(CharsetStr);
    pdest = strstr(CharsetStr, "CHARSET=");
    if ( pdest!= NULL)
    {
        pdest += 8;
        pStr = pdest;
        while(*pdest)
        {
            pdest++;
            len++;
            if(!(*pdest))
            {
                while(*pStr == 0x20)
                {
                    pStr++;
                    len--;
                }
                for(i = 0; i < charsetnum; i++)
                    if (strnicmp(pStr, charset[i], len) == 0)
                        return i;
            }
        }
    }
    if(nType != META_TEXT_VCALE && nType != META_TEXT_VCARD )     
    return CHARSET_UTF8;
    else
        return CHARSET_UNKNOWN;
}

MetaNode *ParseRecvContent(char *pBody, int bodylen,PARSETRELT *pMMS)
{
    int i;
    int nRet, bodynum = 1;//10;
    MMSMultiBody*  mmBody;
    char    headParsed[SIZE_1];
    int     headParsedLen = 0;
    mmsMetaNode *pMetaLast=NULL;
    

    if (pBody == NULL || bodylen <= 0)
        return NULL;
    
    mmBody = (MMSMultiBody*)MMS_malloc(sizeof(MMSMultiBody) 
        + (bodynum - 1)*(sizeof(MMSBody)));
    
    mmBody->Bodynum = bodynum;
    
    nRet = MMS_LibParseMulitBody(pBody, bodylen, mmBody);
    
    if (nRet == MMS_BUFFER_ERR)
    {
        bodynum = mmBody->Bodynum;
        MMS_free(mmBody);
        mmBody = (MMSMultiBody*)MMS_malloc(sizeof(MMSMultiBody) 
            + (bodynum - 1)*(sizeof(MMSBody)));
        mmBody->Bodynum = bodynum;
        nRet = MMS_LibParseMulitBody(pBody, bodylen, mmBody);
        
        if (nRet != MMS_SUCCESS)
        {
            MMS_free(mmBody);
            return pMMS->hMeta;
        }
    }
    for(i = 0; i<mmBody->Bodynum;i++)
    {
        char *FieldValue = NULL;
        unsigned long Len, Offset;
        mmsMetaNode *newNode;
        unsigned char  conttype;
        unsigned char  txtcharset;
        
        headParsedLen = MMS_LibParseBodyHead(mmBody->body[i].pHead, 
            mmBody->body[i].headlen, headParsed, SIZE_1);
        
        headParsed[headParsedLen] = '\0';
        strupr( headParsed );       
        
        if ( FieldValue = 
            URL_GetHeadField(headParsed, HHFN_CONTENTTYPE, &Len, &Offset))
        {
            char *tmp;
            
            tmp =MMS_malloc( Len + 1 );     
            strncpy(tmp, headParsed + Offset, Len);
            tmp[Len] = '\0';    
            conttype = GetMetaType(tmp);
            txtcharset = GetCharset(tmp);
            if (conttype != META_SMIL /*&& conttype != META_OTHER*/)
            {
                newNode = MMS_malloc(sizeof(mmsMetaNode));
                if (!newNode)  
                {
                    MMS_free(tmp);
                    break;
                }
                newNode->inSlide = -1;
                newNode->refCount = 0;
                newNode->MetaType = conttype;
                newNode->txtcharset = txtcharset;
                newNode->Content_Type = tmp;
            }
            else
            {
                newNode = NULL;
                MMS_free(tmp);
            }
        }
        else 
        {
        /*  pNewNode->Content_Type =NULL;
            pNewNode->MetaType = META_OTHER;        
            */
            newNode = NULL;
            conttype = META_OTHER;
            break;          
        }
        
        if (conttype != META_SMIL /*&& conttype != META_OTHER*/)
        {
            if ( FieldValue =
                URL_GetHeadField( headParsed, HHFN_CONTENTID, &Len, &Offset))
            {
                newNode->Content_Id =MMS_malloc( Len + 1 );     
                strncpy(newNode->Content_Id, headParsed + Offset, Len);
                newNode->Content_Id[Len] = 0;
                GetIdContent(newNode->Content_Id, Len);
            }
            else 
                newNode->Content_Id =NULL;
            
            if ( FieldValue = 
                URL_GetHeadField(headParsed, HHFN_CONTENTLOCATION, &Len, &Offset))
            {
                newNode->Content_Location =MMS_malloc( Len + 1 );       
                strncpy(newNode->Content_Location, headParsed + Offset, Len);
                newNode->Content_Location[Len] = 0;     
            }
            else 
                newNode->Content_Location =NULL;        
        
        if (!mmBody->body[i].pBody )
        {
            newNode->Metalen = 0;
            newNode->Metadata = NULL;
        }
        else
        {
            
            newNode->Metalen = mmBody->body[i].bodylen;
            newNode->Metadata = MMS_malloc(newNode->Metalen * 2+1);
            if (!newNode->Metadata)
                newNode->Metalen =0;
            else
            {
                memcpy(newNode->Metadata,mmBody->body[i].pBody,
                newNode->Metalen);
                newNode->Metadata[newNode->Metalen] ='\0';
            }
            if (newNode->MetaType/10 == META_TEXT)
            {
                int len;
                char* pMulti = NULL;
                char* pMetadata = NULL;

                switch(newNode->txtcharset)
                {
                case CHARSET_UTF8:
                    pMulti = MMS_malloc(newNode->Metalen * 2 + 1);

                    newNode->Metalen = UTF8ToMultiByte( 0, 0, newNode->Metadata, 
                        newNode->Metalen, pMulti, newNode->Metalen*2, NULL, NULL);

                    memcpy(newNode->Metadata, pMulti, newNode->Metalen);
                    newNode->Metadata[newNode->Metalen] = '\0';
                    MMS_free(pMulti);
                    break;
                case CHARSET_UCS2:
                    pMulti = MMS_malloc(newNode->Metalen * 2);
                    //delete the fffe  in start??
                    
                    if ((BYTE)newNode->Metadata[0] == 0xff &&
                        (BYTE)newNode->Metadata[1] == 0xfe)
                    {
                        pMetadata = newNode->Metadata + 2;
                        newNode->Metalen -= 2;
                    }
                    len = WideCharToMultiByte(0, 0, (unsigned short*)pMetadata, 
                           newNode->Metalen, pMulti,  newNode->Metalen*2, NULL, NULL);
                    //len = MMS_WideCharToMultiByte((unsigned short*)pMetadata, newNode->Metalen, 
                    //     pMulti,  newNode->Metalen*2);
                    memcpy(newNode->Metadata, pMulti, newNode->Metalen);
                    MMS_free(pMulti);
                    newNode->Metadata[newNode->Metalen] = '\0';
                    break;
                default:
                    break;
                }
                newNode->Metalen = strlen(newNode->Metadata);
            }
        }
    }

        //smil
        if (conttype == META_SMIL)
        {
            char tmp;
            tmp = mmBody->body[i].pBody[mmBody->body[i].bodylen];
            mmBody->body[i].pBody[mmBody->body[i].bodylen] ='\0';
            SMILParse(pMMS->ParseSmil  , mmBody->body[i].pBody,CS_UTF8);
            mmBody->body[i].pBody[mmBody->body[i].bodylen] = tmp;
        }
        
        
        if (newNode)
        {
            newNode->pnext=NULL;
            if (pMMS->hMeta ==NULL)
            {
                pMMS->hMeta  = newNode;
                pMetaLast = newNode;
            }
            else
            {
                pMetaLast->pnext = (MetaNode *)newNode;
                pMetaLast = newNode;                        
            }           
        }
        
    }//end for
    MMS_free(mmBody);
    
    return pMMS->hMeta ;
}//end parsebody

// parse text
MetaNode *ParseText(char *pBody, int bodylen,PARSETRELT *pMMS)
{
    mmsMetaNode *newNode;
    int         conLen;
    char        *tmpContype;
    int         len;
    char        * pMulti = NULL;
    char        * pMetadata = NULL;

    conLen = strlen(pMMS->Parsehead.ConType);
    tmpContype = MMS_malloc(conLen + 1);
    memcpy(tmpContype, pMMS->Parsehead.ConType, conLen);
    tmpContype[conLen] = '\0';

    newNode = MMS_malloc(sizeof(mmsMetaNode));
    if (newNode == NULL)
    {
        MMS_free(tmpContype);
        return NULL;
    }

    newNode->MetaType = GetMetaType(tmpContype);
    newNode->txtcharset = GetCharset(tmpContype);
    newNode->Content_Type = tmpContype;
    newNode->inSlide = -1;
    newNode->refCount = 0;
    newNode->Content_Id = NULL;
    newNode->Content_Location = NULL;
    newNode->Metadata = MMS_malloc(bodylen*2 + 1);
    newNode->Metalen = bodylen;
    memcpy(newNode->Metadata, pBody, bodylen);
    newNode->Metadata[bodylen] = '\0';
    
    switch(newNode->txtcharset)
    {
    case CHARSET_UTF8:
        pMulti = MMS_malloc(newNode->Metalen * 2);
        newNode->Metalen = UTF8ToMultiByte( 0, 0, newNode->Metadata, 
            newNode->Metalen, pMulti, newNode->Metalen*2, NULL, NULL);
        memcpy(newNode->Metadata, pMulti, newNode->Metalen);
        newNode->Metadata[newNode->Metalen] = '\0';
        MMS_free(pMulti);
        break;
    case CHARSET_UCS2:
        pMulti = MMS_malloc(newNode->Metalen * 2);
        
        /*
        if ((newNode->Metadata[0] == 0xff) &&
        (newNode->Metadata[1] == 0xfe))
        {}*/
        pMetadata = newNode->Metadata + 2;
        newNode->Metalen -= 2;
        len = WideCharToMultiByte(0, 0, (unsigned short*)pMetadata, 
            newNode->Metalen, pMulti,  newNode->Metalen*2, NULL, NULL);
        strncpy(newNode->Metadata, pMulti, newNode->Metalen);
        newNode->Metadata[newNode->Metalen] = '\0';
        MMS_free(pMulti);
        newNode->Metadata[newNode->Metalen] = '\0';
        break;
    default:
        break;
    }
    newNode->Metalen = strlen(newNode->Metadata);
    newNode->pnext = NULL;
    
    pMMS->hMeta = newNode;

    return pMMS->hMeta;
}
// parse image
MetaNode *ParseImage(char *pBody, int bodylen,PARSETRELT *pMMS)
{
    mmsMetaNode *newNode;
    int         conLen;
    char        *tmpContype;
    
    conLen = strlen(pMMS->Parsehead.ConType);
    tmpContype = MMS_malloc(conLen + 1);
    memcpy(tmpContype, pMMS->Parsehead.ConType, conLen);
    tmpContype[conLen] = '\0';

    newNode = MMS_malloc(sizeof(mmsMetaNode));
    if (newNode == NULL)
    {
        MMS_free(tmpContype);
        return NULL;
    }

    newNode->MetaType = GetMetaType(tmpContype);
    newNode->Content_Type = tmpContype;
    newNode->inSlide = -1;
    newNode->refCount = 0;
    newNode->Metadata = MMS_malloc(bodylen + 1);
    newNode->Metalen = bodylen;
    memcpy(newNode->Metadata, pBody, bodylen);
    newNode->Metadata[bodylen] = '\0';
    newNode->pnext = NULL;
    pMMS->hMeta = newNode;
    
    return pMMS->hMeta;
}

// parse head and body
PARSETRELT *MMS_Parse(char *pHead, int headLen, char *pBody, int bodyLen)
{
    PARSETRELT *pMMS;
    TAG_SMIL *ParseSmil;
    
    pMMS = MMS_malloc(sizeof(PARSETRELT));
    ParseSmil = MMS_malloc(sizeof(TAG_SMIL));
    
    if (!pMMS || !ParseSmil) 
    {
        MMS_free(ParseSmil);
        MMS_free(pMMS);
        return NULL;
    }
    memset(pMMS, 0, sizeof(PARSETRELT));    
    memset(ParseSmil,0,sizeof(TAG_SMIL));

    pMMS->ParseSmil = ParseSmil;
    pMMS->hMeta = NULL;

    ParseRecvHead(pHead,headLen,&(pMMS->Parsehead));    
    if (stricmp(pMMS->Parsehead.ConType, "application/vnd.wap.multipart.related") == 0 
        || stricmp(pMMS->Parsehead.ConType, "application/vnd.wap.multipart.mixed") == 0 )
        ParseRecvContent(pBody,bodyLen,pMMS); 
    else if(strnicmp(pMMS->Parsehead.ConType, "text/", 5) == 0 )
        ParseText(pBody, bodyLen, pMMS);
    else if(strnicmp(pMMS->Parsehead.ConType, "image/", 6) == 0 )
        ParseImage(pBody, bodyLen, pMMS);
   
    return pMMS;
}

static void FreeParData(int nType, void *pfree)
{
    switch(nType)
    {
    case TT_IMG:
    case TT_TEXT:
    case TT_REF:
        {
            TAG_TEXT *tag = (TAG_TEXT *)pfree;
            MMS_free(tag->src);
            MMS_free(tag->region);
            MMS_free(tag->alt);
            MMS_free(tag->begin);
            MMS_free(tag->end);
        }       
        break;

    case TT_AUDIO:
        {
            TAG_AUDIO* tag = (TAG_AUDIO *)pfree;
            MMS_free(tag->src);
            MMS_free(tag->alt);
            MMS_free(tag->begin);
            MMS_free(tag->end);
        }
        break;

    default:
        break;
    }
    MMS_free(pfree);
}

static void FreeTagData(int TagType,void *pfree)
{
    switch(TagType)
    {
    case TT_REGION:     
        {
            TAG_REGION *tag = (TAG_REGION *)pfree;
            
            MMS_free(tag->left );
            MMS_free( tag->top );
            MMS_free(tag->height );
            MMS_free(tag->width );
            MMS_free( tag->fit );
            MMS_free( tag->id );                
        }
        break;
        
    case TT_ROOT_LAYOUT:
        {
            TAG_ROOTLAYOUT *tag = (TAG_ROOTLAYOUT *)pfree;
            
            MMS_free(tag->height);
            MMS_free(tag->width);
        }
        break;
        
    case TT_META:
        {
            TAG_META *tag = (TAG_META *)pfree;
            MMS_free(tag->name);
            MMS_free(tag->content);         
        }
        break;
        
    case TT_PAR:
        {
            TAG_PAR *tag = (TAG_PAR *)pfree;              
            
            MMS_free(tag->dur);           

            // add 05-16
            if(tag->timg)
                FreeParData(TT_IMG, tag->timg);
            if(tag->taudio)
                FreeParData(TT_AUDIO, tag->taudio);
            if(tag->ttxt)
                FreeParData(TT_TEXT, tag->ttxt);
            if(tag->tref)
                FreeParData(TT_REF, tag->tref);
        }
        break;
        
    default:
        break;
    }
    MMS_free(pfree);
    return;
}

// modify 05-16
void PARSER_FreeSmil(TAG_SMIL *pSmil) 
{
    TAG_META *metaf;
    TAG_PAR  *parf;
    TAG_REGION *regf;
    TAG_ROOTLAYOUT *rootf;
    
    while(pSmil->metah)
    {
        metaf = pSmil->metah;
        pSmil->metah = pSmil->metah->pnext;
        FreeTagData(TT_META,metaf);
    }
    while(pSmil->parh)
    {
        parf = pSmil->parh;
        pSmil->parh = pSmil->parh->pnext;
        FreeTagData(TT_PAR,parf);
    }
    while(pSmil->regionh)
    {
        regf = pSmil->regionh;
        pSmil->regionh = pSmil->regionh->pnext;
        FreeTagData(TT_REGION,regf);
    }
    while(pSmil->rootlayouth)
    {
        rootf = pSmil->rootlayouth;
        pSmil->rootlayouth = pSmil->rootlayouth->pnext;
        FreeTagData(TT_ROOT_LAYOUT,rootf);
    }   
    MMS_free(pSmil);
}

void PARSER_FreePARSETRELT(PARSETRELT *pRelt) 
{
    PARSER_FreeSmil(pRelt->ParseSmil);
    
    while(pRelt->hMeta)
    {
        mmsMetaNode *pnode = pRelt->hMeta;
        pRelt->hMeta = pnode->pnext  ;
        
        MMS_free(pnode->Content_Id);
        MMS_free(pnode->Content_Location);
        MMS_free(pnode->Content_Type);
        MMS_free(pnode->Metadata);      
        MMS_free(pnode);
    }
    MMS_free(pRelt);
}
/*********************************************************************\
**********************************************************************/
void ConvertType(PARSETRELT *pMMS, TotalMMS *pTmms)
{
    mmsMetaNode *tmpMeta;
    int         dur, i;
    TAG_PAR     *tmpPar;
    TAG_REGION  *tmpRegion;
        
    memset(&pTmms->mms.slide, 0, sizeof(SLIDE)*MMS_MAXNUM);
            
    //slides  content-type: application/vnd.wap.multipart.related
    if (strnicmp(pTmms->mms.wspHead.ConType,
            "application/vnd.wap.multipart.related", 37) == 0)
    {
        if (pMMS->ParseSmil == NULL)
            return;
        if (pMMS->ParseSmil->parh == NULL)
            return;

        tmpPar = pMMS->ParseSmil->parh;
        for(i = 0; i < pTmms->mms.wspHead.nSlidnum; i++)
        {
            //slide1 dur
            if (tmpPar->dur != NULL)
            {
                dur = atoi(tmpPar->dur);
                if (strstr(tmpPar->dur, "ms") != NULL)
                    pTmms->mms.slide[i].dur = dur;
                else
                    pTmms->mms.slide[i].dur = dur*1000;
            }
            else
                pTmms->mms.slide[i].dur = DEFAULT_DUR;
            //slide1 image
            pTmms->mms.slide[i].imgAlt[0] = '\0';
            
            if (tmpPar->timg != NULL)
            {               
                char *cidValue = NULL;
                unsigned long Len, Offset;
                char *dupSrc = NULL;
                int  nSrcLen = 0;
                BOOL bCid = FALSE;
                char imgName[MAX_FILENAME];

                if (tmpPar->timg->src != NULL)
                {
                    nSrcLen = strlen(tmpPar->timg->src);
                    dupSrc = MMS_malloc(nSrcLen + 1);
                    strcpy(dupSrc, tmpPar->timg->src);
                    strupr( dupSrc );
                    cidValue = URL_GetHeadField(dupSrc, HHFN_CID, &Len, &Offset);
                    if (cidValue != NULL)
                    {
                        bCid = TRUE;
                        strncpy(imgName, tmpPar->timg->src + Offset, Len);
                        imgName[Len] = 0;
                    }
                    else
                        strcpy(imgName, tmpPar->timg->src);

                    MMS_free(dupSrc);
                }
                //search images
                tmpMeta = pMMS->hMeta;
                while(tmpMeta != NULL)
                {
                    if (tmpMeta->MetaType/10 == META_IMG)  // image
                    {
                        if (!bCid)
                        {
                            if (tmpMeta->Content_Location &&
                                stricmp(tmpMeta->Content_Location, imgName) == 0)   //image
                            {
                                pTmms->mms.slide[i].pImage = tmpMeta;
                                
                                GetImageDimensionFromData(tmpMeta->Metadata, tmpMeta->Metalen, 
                                    &pTmms->mms.slide[i].imgSize);
								MMS_TransferImgSize(&pTmms->mms.slide[i].imgSize);
                                tmpMeta->inSlide = i;
                                tmpMeta->refCount++;
                                break;
                            }
                            else if (tmpMeta->Content_Id && 
                                stricmp(tmpMeta->Content_Id, imgName) == 0)
                            {
                                pTmms->mms.slide[i].pImage = tmpMeta;
                                GetImageDimensionFromData(tmpMeta->Metadata, tmpMeta->Metalen, 
                                    &pTmms->mms.slide[i].imgSize);
								MMS_TransferImgSize(&pTmms->mms.slide[i].imgSize);
                                tmpMeta->inSlide = i;
                                tmpMeta->refCount++;
                                break;
                            }
                        }
                        else if (bCid && tmpMeta->Content_Id != NULL)
                        {
                            if (stricmp(tmpMeta->Content_Id, imgName) == 0) //image
                            {
                                pTmms->mms.slide[i].pImage = tmpMeta;
                                GetImageDimensionFromData(tmpMeta->Metadata, tmpMeta->Metalen, 
                                    &pTmms->mms.slide[i].imgSize);
								MMS_TransferImgSize(&pTmms->mms.slide[i].imgSize);
                                tmpMeta->inSlide = i;
                                tmpMeta->refCount++;
                                break;
                            }
                        }
                        else
                        {
                            pTmms->mms.slide[i].pImage = tmpMeta;                            
                            GetImageDimensionFromData(tmpMeta->Metadata, tmpMeta->Metalen, 
                                &pTmms->mms.slide[i].imgSize);
							MMS_TransferImgSize(&pTmms->mms.slide[i].imgSize);
                            tmpMeta->inSlide = i;
                            tmpMeta->refCount++;
                            break;
                        }
                    }
                    tmpMeta = tmpMeta->pnext;
                }//end image meta

                //////////////////////////////
                tmpRegion = pMMS->ParseSmil->regionh;
                while(tmpRegion != NULL)
                {
                    if (tmpPar->timg->region != NULL &&
                        stricmp(tmpPar->timg->region, tmpRegion->id) == 0)
                    {
                        pTmms->mms.slide[i].imgPoint.x = atoi(tmpRegion->left);
                        pTmms->mms.slide[i].imgPoint.y = atoi(tmpRegion->top);
                        break;
                    }
                    tmpRegion = tmpRegion->pnext;
                }//end image region

                /////////////////////////////
            }//end image

            //slide1 text           
            if (tmpPar->ttxt != NULL)
            {
                char *cidValue = NULL;
                unsigned long Len, Offset;
                char *dupSrc = NULL;
                int  nSrcLen = 0;
                BOOL bCid = FALSE;
                char txtName[MAX_FILENAME];

                if (tmpPar->ttxt->src != NULL)
                {
                    nSrcLen = strlen(tmpPar->ttxt->src);
                    dupSrc = MMS_malloc(nSrcLen + 1);
                    strcpy(dupSrc, tmpPar->ttxt->src);
                    strupr( dupSrc );
                    cidValue = URL_GetHeadField(dupSrc, HHFN_CID, &Len, &Offset);
                    if (cidValue != NULL)
                    {
                        bCid = TRUE;
                        strncpy(txtName, tmpPar->ttxt->src + Offset, Len);
                        txtName[Len] = 0;
                    }
                    else
                        strcpy(txtName, tmpPar->ttxt->src);

                    MMS_free(dupSrc);
                }
                //search txt
                tmpMeta = pMMS->hMeta;
                while(tmpMeta != NULL)
                {
                    if (tmpMeta->MetaType/10 == META_TEXT)    // text
                    {
                        if (!bCid)
                        {
                            if (tmpMeta->Content_Location && 
                                stricmp(tmpMeta->Content_Location, txtName) == 0)   //txt
                            {
                                pTmms->mms.slide[i].pText = tmpMeta;
                                
                                CalTxtSize(tmpMeta->Metadata, &pTmms->mms.slide[i].txtSize);
                                tmpMeta->inSlide = i;
                                tmpMeta->refCount++;
                                break;
                            }
                            else if (tmpMeta->Content_Id && 
                                stricmp(tmpMeta->Content_Id, txtName) == 0) //txt
                            {
                                pTmms->mms.slide[i].pText = tmpMeta;
                                CalTxtSize(tmpMeta->Metadata, &pTmms->mms.slide[i].txtSize);
                                tmpMeta->inSlide = i;
                                tmpMeta->refCount++;
                                break;
                            }
                        }
                        else if (bCid && tmpMeta->Content_Id != NULL)
                        {
                            if (stricmp(tmpMeta->Content_Id, txtName) == 0) //txt
                            {
                                pTmms->mms.slide[i].pText = tmpMeta;
                                CalTxtSize(tmpMeta->Metadata, &pTmms->mms.slide[i].txtSize);
                                tmpMeta->inSlide = i;
                                tmpMeta->refCount++;
                                break;
                            }
                        }
                        else
                        {
                            pTmms->mms.slide[i].pText = tmpMeta;
                            CalTxtSize(tmpMeta->Metadata, &pTmms->mms.slide[i].txtSize);
                            tmpMeta->inSlide = i;
                            tmpMeta->refCount++;
                            break;
                        }
                    }
                    tmpMeta = tmpMeta->pnext;
                }//end text meta

                //////////////////////////////
                tmpRegion = pMMS->ParseSmil->regionh;
                while(tmpRegion != NULL)
                {
                    if (tmpPar->ttxt->region != NULL &&
                        stricmp(tmpPar->ttxt->region, tmpRegion->id) == 0)
                    {
                        if (strchr(tmpRegion->top, '%') == NULL)
                        {
                            pTmms->mms.slide[i].txtPoint.x = atoi(tmpRegion->left);
                            pTmms->mms.slide[i].txtPoint.y = atoi(tmpRegion->top);
                            if (pTmms->mms.slide[i].txtPoint.y == 
                                pTmms->mms.slide[i].imgPoint.y
                                || (pTmms->mms.slide[i].imgPoint.y == 0 && 
                                    pTmms->mms.slide[i].txtPoint.y < 
                                    pTmms->mms.slide[i].imgSize.cy))
                                pTmms->mms.slide[i].txtPoint.y = pTmms->mms.slide[i].imgSize.cy + 2;
                        }
                        else
                        {
                            pTmms->mms.slide[i].txtPoint.x = atoi(tmpRegion->left)*SCREEN_WIDTH/100;
                            pTmms->mms.slide[i].txtPoint.y = pTmms->mms.slide[i].imgSize.cy;                        
                        }
                        break;
                    }
                    tmpRegion = tmpRegion->pnext;
                }//end text region
                if (pTmms->mms.slide[i].imgPoint.y == 0 && 
                    pTmms->mms.slide[i].txtPoint.y < pTmms->mms.slide[i].imgSize.cy)
                    pTmms->mms.slide[i].txtPoint.y = pTmms->mms.slide[i].imgPoint.y +
                                                    pTmms->mms.slide[i].imgSize.cy + 2;
                /////////////////////////////
            }//end text

            //slide1 audio
            if (tmpPar->taudio != NULL)
            {
                char *cidValue = NULL;
                unsigned long Len, Offset;
                char *dupSrc = NULL;
                int  nSrcLen = 0;
                BOOL bCid = FALSE;
                char audioName[MAX_FILENAME];

                if (tmpPar->taudio->src != NULL)
                {
                    nSrcLen = strlen(tmpPar->taudio->src);
                    dupSrc = MMS_malloc(nSrcLen + 1);
                    strcpy(dupSrc, tmpPar->taudio->src);
                    strupr( dupSrc );
                    cidValue = URL_GetHeadField(dupSrc, HHFN_CID, &Len, &Offset);
                    if (cidValue != NULL)
                    {
                        bCid = TRUE;
                        strncpy(audioName, tmpPar->taudio->src + Offset, Len);
                        audioName[Len] = 0;
                    }
                    else
                        strcpy(audioName, tmpPar->taudio->src);

                    MMS_free(dupSrc);
                }
                // search audio
                tmpMeta = pMMS->hMeta;
                while(tmpMeta != NULL)
                {
                    if (tmpMeta->MetaType/10 == META_AUDIO)    // audio
                    {
                        if (!bCid )
                        {
                            if (tmpMeta->Content_Location && 
                                stricmp(tmpMeta->Content_Location, audioName) == 0) //audio
                            {
                                pTmms->mms.slide[i].pAudio = tmpMeta;
                                tmpMeta->inSlide = i;
                                tmpMeta->refCount++;
                                break;
                            }
                            else if (tmpMeta->Content_Id && 
                                stricmp(tmpMeta->Content_Id, audioName) == 0)   //audio
                            {
                                pTmms->mms.slide[i].pAudio = tmpMeta;
                                tmpMeta->inSlide = i;
                                tmpMeta->refCount++;
                                break;
                            }
                        }
                        else if (bCid && tmpMeta->Content_Id != NULL)
                        {
                            if (stricmp(tmpMeta->Content_Id, audioName) == 0)   //audio
                            {
                                pTmms->mms.slide[i].pAudio = tmpMeta;
                                tmpMeta->inSlide = i;
                                tmpMeta->refCount++;
                                break;
                            }
                        }
                        else
                        {
                            pTmms->mms.slide[i].pAudio = tmpMeta;
                            tmpMeta->inSlide = i;
                            tmpMeta->refCount++;
                            break;
                        }
                    }
                    tmpMeta = tmpMeta->pnext;
                }
            }//end audio
            tmpPar = tmpPar->pnext;
        }//end for
    }// end related content type
    
    //if the type is mixted, application/vnd.wap.multipart.mixed
    else 
        //if (strnicmp(pTmms->mms.wspHead.ConType,
        //  "application/vnd.wap.multipart.mixed", 35) == 0)
    {       
                    
        tmpMeta = pMMS->hMeta;
        i = 0;
        while(tmpMeta != NULL)
        {
            pTmms->mms.slide[i].dur = DEFAULT_DUR;
            if (tmpMeta->MetaType/10 == META_IMG)   //image
            {
                pTmms->mms.slide[i].pImage = tmpMeta;

                pTmms->mms.slide[i].imgAlt[0] = '\0';
                
                GetImageDimensionFromData(tmpMeta->Metadata, tmpMeta->Metalen, 
                        &pTmms->mms.slide[i].imgSize);
                tmpMeta->inSlide = i;
                tmpMeta->refCount++;
            }//end image
            else if (tmpMeta->MetaType/10 == META_AUDIO)//audio
            {   
                pTmms->mms.slide[i].pAudio = tmpMeta;

                tmpMeta->inSlide = i;
                tmpMeta->refCount++;
            }//end audio
            else if (tmpMeta->MetaType/10 == META_TEXT) //text
            {
                pTmms->mms.slide[i].pText = tmpMeta;
                
                CalTxtSize(tmpMeta->Metadata, &pTmms->mms.slide[i].txtSize);
                pTmms->mms.slide[i].txtPoint.y = pTmms->mms.slide[i].imgSize.cy+2;
                tmpMeta->inSlide = i;
                tmpMeta->refCount++;
            }//end text
            tmpMeta = tmpMeta->pnext;
            i++;
            if (i >= MMS_MAXNUM)
                break;
        }//end while

        if (i > 0)
            pMMS->ParseSmil->nParNum = i;
        else
            pMMS->ParseSmil->nParNum = 1;
    }//end mixed

}//end

static int HexStrToInt(char * s)
{
    int val = 0;
    char c;
    int i, len;

    len = strlen(s);
    for (i = 0; i < len; i++)
    {
        c = s[i];
        if (ISNUMBER(c))
            val = val * 16 + (c - '0');
        else if (c >= 'a' && c <= 'f')
            val = val * 16 + (c - 'a' + 10);
        else
            val = val * 16 + (c - 'A' + 10);
    }

    return val;
}
#define ARR_LEN 512 *2

/*********************************************************************\
* Function     utf8 to gb2312
* Params       txt: utf8 src
* Return           void
**********************************************************************/
/*void MMSPro_UTF8toGB2312(unsigned char * txt)
{
    unsigned char *ptr;
    unsigned char *ptr2;
    unsigned char *start;
    unsigned short awUnic[ARR_LEN];
    int             idx, ret;
    int             len;

    char str[7];//change by linquan 04/08/2002

    if (txt == NULL)
        return;
    
    ptr = txt;
    ptr2 = txt;

    if (*ptr == '\0')
        return;
            
    while (*ptr != '\0')
    {

        /* 1110xxxx 10xxxxxx 10xxxxxx utf8code */
        /* 110xxxxx 1110xxxx 11110xxx 111110xx 1111110x start code*/
        /* POSSIBLE LEGAL START 0XXXXXXX(ASCII) 11000000 (?) 11100000(simple chinese) */
        
/*      while((*ptr &  MUTF8_ASCII_MASK) == MUTF8_ASCII_RESULT)
        {
            ptr++;
            ptr2++;
            if (!*ptr) return;
        }

        //掠过不合法序列
        while(( (*ptr & MUTF8_FOLLOW_MASK)==MUTF8_FOLLOW_RESULT)  ||
             ((*ptr & MUTF8_ILLSTART_MASK)==MUTF8_ILLSTART_RESULT) )
        {
            ptr++;
            if (!*ptr)
            {
                *ptr2 = '\0';
                ptr =ptr2;
                return;
            }
        }
        
        //有非法序列
        if (ptr != ptr2) 
        {
            strcpy(ptr2,ptr);
            ptr = ptr2;
            continue;           
        }
        
        idx = 0;
        len =UTF8Process(ptr);
        while(len != 0)
        {
            char    c;
            switch( len)
            {
            case 2:
                /*start = ptr;

                c = (*start>>4) & 0x0f;
                str[0] = CONVERT(c);
                
                c = *start & 0x0f;
                str[1] = CONVERT(c);
                str[2] = 0;
                awUnic[idx] = (unsigned short)HexStrToInt(str);               
                idx++;
                
                ptr+=2;
                break;*/
/*          case 4:     

                start = ptr;
                c = (*start>>4) & 0x0f;
                str[0] = CONVERT(c);
                
                c = *start & 0x0f;
                str[1] = CONVERT(c);
                
                c = (*(start+1)>>4) & 0x0f;
                str[2] = CONVERT(c);
                
                c = *(start+1) & 0x0f;
                str[3] = CONVERT(c);
                
                str[4] = 0;
                
                awUnic[idx] = (unsigned short)HexStrToInt(str);
                idx++;
                
                if (len == 2) ptr +=2;
                else ptr+=3;
                break;
            default:
            
                break;
            }   
            len =UTF8Process(ptr);

        }
        //如果有序列
        if (idx !=0)
        {
            awUnic[idx] = 0;
            idx++;
            ret = MMS_WideCharToMultiByte(awUnic, idx, ptr2, idx * 2);
            
            if (ret > 0)
                ptr2 += ret;
            strcpy(ptr2,ptr);
            ptr = ptr2;
        }
        
    }

}//end 

static int UTF8Process(unsigned char *start)
{
    char *pcur,*pstart;
    int  i,codeindex =0;
    int  codelen =0;
    BOOL    boverlong =FALSE,billform =FALSE;

    pstart = start;
    pcur = start;
    
    for (i = 0;i <CODETABLELEN; i ++)
    {
        if ((*pcur & utf_mask[i].judgemask) == utf_mask[i].judgeresult)
        {
            codeindex =i;
            codelen = i+2;
            break;
        }
    }

    if ((codeindex == 0 )&& (codelen ==0))
        return 0;

    //是否过长
    if (utf_mask[codeindex].illb2mask == NULL)
    {
        if ((*pcur & utf_mask[codeindex].illb1mask) == utf_mask[codeindex].illb1result)
            boverlong = TRUE;
    }
    else
    {
        if (((*pcur & utf_mask[codeindex].illb1mask) == utf_mask[codeindex].illb1result)
            &&(((*(pcur+1)) & utf_mask[codeindex].illb2mask) == utf_mask[codeindex].illb2result))
            boverlong = TRUE;
    }

    if (boverlong) 
    {
        *pstart =(char)MUTF8_FOLLOW_RESULT;
        return 0;
    }

    //是否长度符合
    for (i =1;i < codelen;i++)
    {
        pcur++;
        if (( *pcur & MUTF8_FOLLOW_MASK) != MUTF8_FOLLOW_RESULT)
        {
            *pstart = (char)MUTF8_FOLLOW_RESULT;
            return 0;
    
        }
    }

    //以上证明合法，开始转换
    switch( codelen)
    {
    case 2:
        pcur = pstart;
        *(pcur+1) = ((*pcur & 0x03)<<6)| (*(pcur+1) & 0x3f ) ;

        *pcur = (*pcur & 0x3c) >> 2;
        
        return 2;

        break;
    case 3: 
    
        pcur = pstart;
        *pcur = (*pcur & 0x0f) << 4;
        
        *pcur |= (*(pcur+1) & 0x3c) >> 2 ;
        *pcur++;
        *pcur = ((*pcur & 0x03) << 6) | (*(pcur+1) & 0x3f);
        *pcur++;
        *pcur = 0;
        return 4;
        break;
    case 4:
    case 5:
    case 6:
    default:
        *pstart = (char)MUTF8_FOLLOW_RESULT;
        return 0;
        break;
        
    }

    return 0;
}

int MMS_MultiByteToWideChar( char *lpMultiByteStr, int cchMultiByte,char  *lpWideCharStr, int cchWideChar )
{
    int nChars;                     //生成的目标字串的字符数目
    int nBytes;                     //需要转换成unicode的编码数目，因为需要再转换成utf8每个编码需要多一个存储字节           
    char* lpSrc = NULL;
    char* lpTmp = NULL;
    int bResult = 0;
    char* lpDst = NULL;
    LONG lDistance;
    WORD WRec[1];
    unsigned short WRecH,WRecL;
    BOOL bForSize = FALSE;
#ifdef WB_FOR_MSWIN
    FILE *fp;
#else
    HANDLE hFile;
#endif


    if ( lpMultiByteStr == 0 )  /* null pointer，function fails */
/*  {
        MsgOut("null pointer，char-convert function fails!\n");
        return 0;
    }

    if ( cchWideChar == 0 ) /* to get the required buffer size in wide characters */
/*      bForSize = TRUE;

    if ( !bForSize &&  lpWideCharStr == 0 ) /* null pointer，function fails */
/*  {
        MsgOut("null pointer，char-convert function fails!\n");
        return 0;
    }

    if ( !bForSize )
    {
        lpDst = lpWideCharStr;      
    }

    /***********************************************************************
     *
     * In GBK scope:
     *      high byte of the multi-byte character is between 0x81 and 0xfe, 
     *      low byte of the multi-byte character is between 0x40 and 0xfe. 
     *
     **********************************************************************/
    /* open char mapping file */
/*#ifdef WB_FOR_MSWIN
    fp = fopen("gb2uni.cod", "rb");
    if (fp == NULL)
        return 0;

#else 
/*  hFile = CreateFile(UNICODE_PATH"gb2uni.cod", ACCESS_READ, FILE_ATTRIBUTE_NORMAL);
    if (hFile == INVALID_HANDLE_VALUE)
        return 0;
#endif

    lpSrc = (char *)lpMultiByteStr;
    nBytes = 0;
    nChars = 0;
    while ( *lpSrc != '\0' )
    {
        if ( (BYTE)*lpSrc >= 0xa0 && (BYTE)*lpSrc <= 0xfe )
        {
            lpTmp = lpSrc;
            lpTmp++;
            if ( (BYTE)*lpTmp >= 0xa0 && (BYTE)*lpTmp <= 0xfe ) /* char in GB2312 scope */
/*          {
                if ( !bForSize )
                {
                    lDistance = ((BYTE)*lpSrc - 0x81)*191*2 + ((BYTE)*lpTmp - 0x40)*2;
#ifdef WB_FOR_MSWIN
                    fseek(fp, lDistance, SEEK_SET);
                    bResult = fread(WRec, 2, 1, fp1);
#else
                    SetFilePointer(hFile, lDistance, SEEK_SET);
                    bResult = ReadFile(hFile, WRec, 2);
#endif
                    WRecH = MMS_HIBYTE(WRec[0]);
                    WRecL = MMS_LOBYTE (WRec[0]);
                    *(lpDst++) = (char)WRecH;
                    *(lpDst++) = (char)WRecL;
                    
                }
                lpSrc++;
                lpSrc++;
                nBytes++;
                nChars +=1;
            }
            else
            {
                if ( !bForSize )
                {
                    *(lpDst++) = (*lpSrc);
                }
                lpSrc++;
            }
        }
        else    /* char in ASCII scope */
/*      {
            if ( !bForSize )
            {
                *(lpDst++) = (*lpSrc);
            }
            lpSrc++;
        }
        
        nChars++;
//      if ( nBytes >= cchMultiByte )
//          break;
    }
#ifdef WB_FOR_MSWIN
    fclose(fp);
#else
    CloseFile(hFile);   /* close char mapping file */
/*#endif

    if ( !bForSize )
    {
        *lpDst = 0;
    }

    return nBytes;
}


/*
 * this function first valid the parameters;
 *
 * if (cchWideChar == -1), then we need calculate the strlen of lpWideCharStr,
 * which must be ended with 0x0000. we need this value to decide the bytes of lpResult.
 *
 * then we MMS_malloc a str, lpResult, to record the convert result for multibyte,
 * and at the same time, count the bytes be converted; before we return, copy the converted
 * result, lpResult, to lpMultiByteStr.
 *
 * if (cchMultiByte == -1), then we needn't MMS_malloc str lpResult,
 * just count the bytes be converted.
 *
 * if the bytes be converted is larger than cchMultiByte, then error occured.
 *
 * the return value is the bytes be converted if success, 0 if fail.
 */
int MMS_WideCharToMultiByte(unsigned short * lpWideCharStr, int cchWideChar, 
                           char * lpMultiByteStr, int cchMultiByte)
{
    unsigned short table[TableSize] = 
    {   
        0x0, 0x1, 0x2, 0x3, 0x4, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25,
        0x26, 0x30, 0x31, 0x32, 0x33, 0xF9, 0xFA, 0xFE, 0xFF
    };

    unsigned short wtemp;

#ifdef WB_FOR_MSWIN
    FILE *fp1, *fp2;
#else
    int  hFile1, hFile2;
#endif

    //use to the flag whether need to return the converted result.
    int flag;

    //record the number in widechar to be handled.
    int wcount = (cchWideChar == -1) ? w_wcslen(lpWideCharStr) : cchWideChar;

    //lpResult is used to record the converted result.
    //if cchMultiByte be zero, then lpResult will be no use.
    char * lpResult = NULL; 

    //to record how many multibytes be converted when this function returned, that is,
    //the number of valid bytes the multibyte string will have.
    //if (flag == TRUE), it also record the pointer of lpResult during in this function.
    int cConverted = 0;
    int bResult = 0;
    int i = 0;
    unsigned char buffer1[NUM_IN_FIELD_ZERO * 4];
    int Buf1Size = NUM_IN_FIELD_ZERO * 4;
    unsigned char * buffer2 = NULL;
    unsigned int filesize;
    int         indexGsm;
    BOOL        isGsm = FALSE;

    if (cchMultiByte == 0)
        flag = 0;
    else
        flag = 1;

    if ( flag )
    {
        int buf_size = wcount * 2 + 1;
        lpResult = (char *) MMS_malloc(buf_size);
        if (lpResult == NULL)
            return 0;
        memset( lpResult, 0, buf_size);
    }

    /* open files */
#ifdef WB_FOR_MSWIN
    //fp1 = fopen(UNICODE_PATH"uni2gb.cod", "rb");
    fp1 = fopen("uni2gb.cod", "rb");
    if (fp1 == NULL)
        return 0;

#else 
    hFile1 = open(UNICODE_PATH"uni2gb.cod", O_RDONLY);
    if (hFile1 == -1)
        return 0;
#endif

    while ( (wtemp = *lpWideCharStr) && (wcount) )
    {
        int case_no = -1;

        lpWideCharStr++;
        wcount--;

        /* according to the high byte (sometimes with low byte) of UNICODE,
         * switch cases to 0, 1, 2, 3, 4.
         *  0: the high byte is 0x0, it is dealed alone;
         *  1: the UNICODE is in range of 0x4EOO to 0x9FFF;
         *  2: the UNICODE is in range of 0xE000 to 0xE8FF;
         *  3: the high byte is one of elements in table[];
         *      NOTE:   if the high byte is in table[], and the UNICODE can be found 
         *              in the file "other_gb.cod", then it will be dealed.
         *              while if the UNICODE can't be found in the file "other_gb.cod",
         *              then it will go to case 4 and then be handled.
         *  4: others.
         */
        if (MMS_HIBYTE(wtemp) == 0 || MMS_HIBYTE(wtemp) == 0x03) 
            case_no = 0;
        else if ( (wtemp >= 0x4E00) && (wtemp <= 0x9FFF) ) 
            case_no = 1;
        else if ( (wtemp >= 0xE000) && (wtemp <= 0xE8FF) ) 
            case_no = 2;
        else
        {
            for (i = 0; i < TableSize; i++)
            {
                if (MMS_HIBYTE(wtemp) == table[i]) 
                    break;
            }
            
            if (i < TableSize)
                case_no = 3;
            else
                case_no = 4;    //use the default char '?'
        }
        
        switch (case_no)
        {
        case 0:
            {
                unsigned char lowbyte = MMS_LOBYTE(wtemp);
                //if low byte is more than 0x7F, then check in the file "other_gb.cod"
                //to find whether it is in the file.
                //if does, got the multibyte in the following two bytes,
                //else, looked as ASCII code, that is, only copy the low byte.

                if (lowbyte >= 0x80)
                {
                ////////////////////////////////////////////////
                isGsm = FALSE;
                for (indexGsm = 0; indexGsm < MAX_GSM_NUM; indexGsm++)
                {
                    if (wtemp == UniCode[indexGsm])
                    {
                        isGsm = TRUE;
                        break;
                    }
                }
                
                if (isGsm)
                {
                    *(lpResult +cConverted) = (char)GsmCode[indexGsm];                    
                    cConverted += 1;
                    
                    break;
                }
                ////////////////////////////////////////////////

                    /*add by linquan for nbsp char begin 10/22/2002 */
                    if (lowbyte == 0xa0 )               
                    {
                        lpResult[cConverted++] = 0x20;
                        break;
                    }
                    else if (lowbyte == 0xad )              
                    {
                        lpResult[cConverted++] = 0x2d;
                        break;
                    }
                    /*add by linquan for nbsp char end 10/22/2002 */
                    
#ifdef WB_FOR_MSWIN
                    fp2 = fopen("other_gb.cod", "rb");
                    if (fp2 == NULL)
                    {
                        break;//break case 2
                    }
                    bResult = fread(buffer1, Buf1Size, 1, fp2);

#else
                    hFile2 = open(UNICODE_PATH"other_gb.cod", O_RDONLY);

                    if (hFile2 == -1)
                    {
                        break; //break case 2
                    }
                    bResult = read(hFile2, buffer1, Buf1Size);

#endif
                    i = 0;
                    while( (buffer1[i] < lowbyte) && (buffer1[i + 1] == 0x0) && 
                           (i < Buf1Size))
                    {
                        i += 4;
                    }

                    //found it in range 0 in the file "other_gb.cod"
                    if ((buffer1[i] == lowbyte ) && (buffer1[i + 1] == 0x0))
                    {
                        if (flag)
                        {
                            lpResult[cConverted++] = buffer1[i + 2];
                            lpResult[cConverted++] = buffer1[i + 3];
                        }
                        else
                            cConverted += 2;
                    }
                    //if else, deal it the same as ASCII,
                    //like those codes with low byte less than 0x80.
#ifdef WB_FOR_MSWIN
                    fclose(fp2);
#else
                    close(hFile2);
#endif
                }
                else  // deal as ASCII code
                {
                    if (flag)
                        lpResult[cConverted++] = lowbyte;
                    else
                        cConverted += 1;
                }
            }
            break;  //break case 0.

        case 1:
            if (!flag)
            {
                cConverted += 2;
            }
            else
            {
                int offset = (wtemp - 0x4E00) * 2; /* how to use it? */
                
#ifdef WB_FOR_MSWIN
                fseek(fp1, offset, SEEK_SET);
                bResult = fread(lpResult + cConverted, 2, 1, fp1);
#else
                lseek(hFile1, offset, SEEK_SET);
                bResult = read(hFile1, lpResult + cConverted, 2);
#endif

                cConverted += 2;
            }
            break;
            
        case 2:
            if (!flag)
            {
                cConverted += 2;
            }
            else
            {
                int offset = (wtemp - 0xE000 + 0x5200) * 2;
                
#ifdef WB_FOR_MSWIN
                fseek(fp1, offset, SEEK_SET);
                bResult = fread(lpResult + cConverted, 2, 1, fp1);
#else
                lseek(hFile1, offset, SEEK_SET);
                bResult = read(hFile1, lpResult + cConverted, 2);
#endif

                cConverted += 2;
            }
            break;
            
        case 3:
            {
                unsigned char low;
                unsigned short wval = 0x0;
                int start = 0, end, point = 0;

#ifdef WB_FOR_MSWIN
                fp2 = fopen("other_gb.cod", "rb");
                if (fp2 == NULL)
                {
                    if (flag)
                        lpResult[cConverted++] = '?';
                    else
                        cConverted++;

                    break;//break case 3
                }

                fseek(fp2, 0, SEEK_END);
                filesize = ftell(fp2);
                fseek(fp2, 0, SEEK_SET);
#else
                hFile2 = open(UNICODE_PATH"other_gb.cod", O_RDONLY);

                if (hFile2 == -1)
                {
                    if (flag)
                        lpResult[cConverted++] = '?';
                    else
                        cConverted++;
                    break; //break case 3
                }

                filesize = MMS_GetFileSize(UNICODE_PATH"other_gb.cod");

#endif
                if (buffer2 == NULL)
                    buffer2 = (char *)MMS_malloc(filesize);

#ifdef WB_FOR_MSWIN
                bResult = fread(buffer2, filesize, 1, fp2);
#else
                bResult = read(hFile2, buffer2, filesize);
#endif
                start = 0;
                end   = filesize;

                while( (end - start) > 4) /* no abs, ok? */
                {
                    point = ( start + end ) / 2;
                    point &= 0xFFFC;    //align to 4 byte

                    //if not assign to a BYTE var, then there will be error
                    //when doing or operation.
                    low = buffer2[point];
                    wval = (buffer2[point + 1] << 8) | low;

                    if (wval == wtemp)
                    {
                        //found it, break from while
                        break;  //break while( abs(end - start) > 4)
                    }
                    else
                        if ( wval > wtemp ) 
                            end = point;
                        else 
                            start = point;
                }

                if ( (end - start) > 4 )    //found it
                {
                    if (flag)
                    {
                        lpResult[cConverted++] = buffer2[point + 2];
                        lpResult[cConverted++] = buffer2[point + 3];
                    }
                    else
                    {
                        cConverted += 2;
                    }
                }
                else  /* not found */
                {
                    if (flag)
                        lpResult[cConverted++] = '?';
                    else
                        cConverted++;
                }

#ifdef WB_FOR_MSWIN
                fclose(fp2);
#else
                close(hFile2);
#endif
            }
            break;

        case 4:
            if (flag)
                lpResult[cConverted++] = '?';
            else
                cConverted++;

            break;
            
        default:
            return 0;

        }   //end of switch (case_no)
    }   //end of while ( (wtemp = *lpWideCharStr++) && (wcount--) )

    if ( flag )
    {
        if ( cchMultiByte >= cConverted )
            strcpy( lpMultiByteStr, lpResult );

        MMS_free( lpResult );
    }

    if (buffer2 != NULL)
        MMS_free(buffer2);

    /* close files */
#ifdef WB_FOR_MSWIN
    fclose(fp1);
#else
    close(hFile1);
#endif

    return cConverted;
}

BOOL MMS_TransferImgSize(SIZE* pSize)
{
	int imgW, imgH;

	imgW = pSize->cx;
	
	pSize->cx = imgW < SCREEN_WIDTH ? imgW : SCREEN_WIDTH;
	
	pSize->cy = pSize->cy * pSize->cx/imgW;

	imgH = pSize->cy;

	pSize->cy = imgH < SCREEN_HEIGHT ? imgH : SCREEN_HEIGHT;

	if(pSize->cy == SCREEN_HEIGHT)
		pSize->cx = pSize->cx * SCREEN_HEIGHT/imgH;

	return TRUE;
}

/*****************************************************************************
 *
 *  Char code convert routines:
 *
 *****************************************************************************
 */

/*int MMS_MultiByteToWideCharToUtf8( char *lpMultiByteStr, int cchMultiByte,
char  *lpWideCharStr, int cchWideChar )
{
    int nChars;                     
    int nBytes;                     
    char* lpSrc = NULL;
    char* lpTmp = NULL;
    int bResult = 0;
    char* lpDst = NULL;
    LONG lDistance;
    WORD WRec[1];
    BYTE Butf8[3];
    unsigned short WRecH,WRecL;
    BOOL bForSize = FALSE;
    BOOL isGsm = FALSE;

#ifdef WB_FOR_MSWIN
    FILE *fp;
#else
    HANDLE hFile;
#endif


    if ( lpMultiByteStr == 0 )  /* null pointer，function fails */
/*  {
        MsgOut("null pointer，char-convert function fails!\n");
        return 0;
    }

    if ( cchWideChar == 0 ) /* to get the required buffer size in wide characters */
/*      bForSize = TRUE;

    if ( !bForSize &&  lpWideCharStr == 0 ) /* null pointer，function fails */
/*  {
        MsgOut("null pointer，char-convert function fails!\n");
        return 0;
    }

    if ( !bForSize )
    {
        lpDst = lpWideCharStr;      
    }

    /***********************************************************************
     *
     * In GBK scope:
     *      high byte of the multi-byte character is between 0x81 and 0xfe, 
     *      low byte of the multi-byte character is between 0x40 and 0xfe. 
     *
     **********************************************************************/
    /* open char mapping file */
/*#ifdef WB_FOR_MSWIN
    fp = fopen("gb2uni.cod", "rb");
    if (fp == NULL)
        return 0;

#else 
/*  hFile = CreateFile(UNICODE_PATH"gb2uni.cod", ACCESS_READ, FILE_ATTRIBUTE_NORMAL);
    if (hFile == INVALID_HANDLE_VALUE)
        return 0;
#endif

    lpSrc = (char *)lpMultiByteStr;
    nBytes = 0;
    nChars = 0;
    while ( *lpSrc != '\0' )
    {
        if ( (BYTE)*lpSrc >= 0xa0 && (BYTE)*lpSrc <= 0xfe )
        {
            lpTmp = lpSrc;
            lpTmp++;
            if ( (BYTE)*lpTmp >= 0xa0 && (BYTE)*lpTmp <= 0xfe ) /* char in GB2312 scope */
/*          {
                if ( !bForSize )
                {
                    lDistance = ((BYTE)*lpSrc - 0x81)*191*2 + ((BYTE)*lpTmp - 0x40)*2;
#ifdef WB_FOR_MSWIN
                    fseek(fp, lDistance, SEEK_SET);
                    bResult = fread(WRec, 2, 1, fp1);
#else
                    SetFilePointer(hFile, lDistance, SEEK_SET);
                    bResult = ReadFile(hFile, WRec, 2);
#endif
                    WRecH = MMS_HIBYTE(WRec[0]);
                    WRecL = MMS_LOBYTE (WRec[0]);
                    Butf8[0] = 0XE0 | (WRecH >> 4 );
                    Butf8[1] = 0X80 | ((WRecH & 0X0F)<<2) | ((WRecL & 0XD0 )>>6) ;
                    Butf8[2] = 0X80 | (0X3F & WRecL);       
                    *(lpDst++) = Butf8[0];
                    *(lpDst++) = Butf8[1];
                    *(lpDst++) = Butf8[2];
                }
                lpSrc++;
                lpSrc++;
                nBytes++;
                nChars +=2;
            }
            else
            {
                if ( !bForSize )
                {
                    *(lpDst++) = (*lpSrc);
                }
                lpSrc++;
            }
        }
        else
        {
            //////////////////////////////////////////////////////////////////////////
            // special char
            if ( (BYTE)*lpSrc >= 0x00 && (BYTE)*lpSrc <= 0x8c )
            {
                int i;
                WORD GsmUnicode;
                
                isGsm = FALSE;
                for (i = 0; i<MAX_GSM_NUM; i++)
                {
                    if ((BYTE)*lpSrc == GsmCode[i])
                    {
                        isGsm = TRUE;
                        break;
                    }
                }

                if (!bForSize)
                {
                    if (isGsm)
                    {
                        GsmUnicode = UniCode[i];
                        WRecH = MMS_HIBYTE(GsmUnicode);
                        WRecL = MMS_LOBYTE (GsmUnicode);
                        Butf8[0] = 0xC0 | ((WRecH & 0X0F)<<2) | ((WRecL & 0XC0 )>>6) ;
                        Butf8[1] = 0X80 | (0X3F & WRecL);       
                        *(lpDst++) = Butf8[0];
                        *(lpDst++) = Butf8[1];
                    }
                    else
                    {
                        *(lpDst++) = (*lpSrc);
                    }
                }
                
                if (isGsm)
                {
                    lpSrc++;
                    nBytes++;
                    nChars++;
                }
                else
                    lpSrc++;
            }
            ////////////////////////////////////////////////            

        }
        
        nChars++;
//      if ( nBytes >= cchMultiByte )
//          break;
    }
#ifdef WB_FOR_MSWIN
    fclose(fp);
#else
    CloseFile(hFile);   /* close char mapping file */
/*#endif

    if ( !bForSize )
    {
        *lpDst = 0;
    }

    return nBytes;
}
*/
