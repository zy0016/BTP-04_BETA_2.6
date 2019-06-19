#include "ABGlobal.h"

BOOL AB_IsFlashEnough(void)
{
    return TRUE;
}

int AB_GetContactCount(void)
{
    return nName;
}
/*********************************************************************\
* Function	Test_GenChain
* Purpose   
* Params	
* Return	
* Remarks
**********************************************************************/
BOOL Test1_GenChain(PCONTACT_ITEMCHAIN *ppItem)
{
    CONTACT_ITEMCHAIN *pNewItem;
    int i;

//AB_TYPEMASK_FIRSTNAME     
//AB_TYPEMASK_LASTNAME      
//AB_TYPEMASK_TEL           
//AB_TYPEMASK_TELHOME       
//AB_TYPEMASK_TELWORK       
//AB_TYPEMASK_MOBILE        
//AB_TYPEMASK_MOBILEHOME    
//AB_TYPEMASK_MOBILEWORK    
//AB_TYPEMASK_FAX           
//AB_TYPEMASK_PAGER         
//AB_TYPEMASK_EMAIL         
//AB_TYPEMASK_EMAILWORK     
//AB_TYPEMASK_EMAILHOME     
//AB_TYPEMASK_POSTALADDRESS 
//AB_TYPEMASK_HTTPURL       
//AB_TYPEMASK_JOBTITLE      
//AB_TYPEMASK_COMPANY       
//AB_TYPEMASK_COMPANYTEL    
//AB_TYPEMASK_COMPANYADDRESS
//AB_TYPEMASK_DTMF          
//AB_TYPEMASK_DATE          
//AB_TYPEMASK_NOTE       
    
    for( i=0 ; i<AB_EDITCONTACT_CTRNUM ; i++ )
    {
        pNewItem = Item_New(Contact_Item[i].dwMask);
        
        if(pNewItem == NULL)
            return FALSE;
        
        Item_Insert(ppItem,pNewItem);
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
void AB_PleaseToWait(void)
{
    PLXTipsWin(NULL,NULL,0,"Please waiting...we \r\n will correct this \r\n problem in \r\nAlpha1.0","Prompt",
        Notify_Info,IDS_OK,NULL,WAITTIMEOUT);
}

//#define _TEST_APP_GetQuickDial_
//#define _TEST_APP_GetInfoByPhone_
//#define _TEST_APP_GetNameByPhone_
//#define _TEST_APP_GetGroupInfo_
#define _TEST_APP_GetOnePhoneOrEmail_

void Test(HWND hWnd)
{
//    BOOL bRet;

#ifdef _TEST_APP_GetQuickDial_
    int i;
    ABNAMEOREMAIL name;

    memset(&name,0,sizeof(ABNAMEOREMAIL));

    for(i = 0 ; i <= 9 ; i++)
    {
        bRet = APP_GetQuickDial((WORD)i,&name);

        if(bRet == TRUE)
            printf("\r\n Quick no is %d, the name is %s ,the tel is %s:)\r\n",i,name.szName,name.szTelOrEmail);
        else
            printf("\r\n fail to quick dial :(\r\n");
    }
#endif

#ifdef _TEST_APP_GetInfoByPhone_
    ABINFO abinfo;

    memset(&abinfo,0,sizeof(ABINFO));
    
    bRet = APP_GetInfoByPhone("12345",&abinfo);
    
    if(bRet == TRUE)
        printf("\r\n get no is %s, the name is %s ,the ring is %s,the icon is %s:)\r\n",
        abinfo.szTel,abinfo.szName,abinfo.szRing,abinfo.szIcon);
    else
        printf("\r\n fail to getinfobyphone :(\r\n");
#endif


#ifdef _TEST_APP_GetNameByPhone_
    int i;
    ABNAMEOREMAIL name;
    
    memset(&name,0,sizeof(ABNAMEOREMAIL));
    
    bRet = APP_GetNameByPhone("12345",&name);
    
    if(bRet == TRUE)
        printf("\r\n no is %s, the name is %s\r\n",name.szTelOrEmail,name.szName);
    else
        printf("\r\n fail to getnamebyphone :(\r\n");

    
    memset(&name,0,sizeof(ABNAMEOREMAIL));
    
    bRet = APP_GetNameByEmail("a@b.c",&name);
    
    if(bRet == TRUE)
        printf("\r\n email is %s, the name is %s\r\n",name.szTelOrEmail,name.szName);
    else
        printf("\r\n fail to getnamebyemail :(\r\n");
#endif


#ifdef _TEST_APP_GetGroupInfo_
    int nGroup;
    GROUP_INFO* pGroup_Info = NULL;
    
    nGroup = 0;
    if(APP_GetGroupInfo(NULL,&nGroup))
    {
        if(nGroup != 0)
        {
            pGroup_Info = malloc(sizeof(GROUP_INFO)*nGroup);

            if(pGroup_Info == NULL)
                return FALSE;

            memset(pGroup_Info,0,sizeof(GROUP_INFO));

            bRet = APP_GetGroupInfo(pGroup_Info,&nGroup);
        }
    }
#endif

#ifdef _TEST_APP_GetOnePhoneOrEmail_
    ABNAMEOREMAIL* p;

    p = (ABNAMEOREMAIL*)malloc(sizeof(ABNAMEOREMAIL)*10);

    memset(p,0,sizeof(ABNAMEOREMAIL)*10);

    p->nType = AB_NUMBER;
    strcpy(p->szName,"Q");
    strcpy(p->szTelOrEmail,"12345");
    APP_GetOnePhoneOrEmail(hWnd,NULL,0,PICK_NUMBER);
//    APP_GetMultiPhoneOrEmail(hWnd,hWnd,WM_USER+100,PICK_NUMBER,p,1,10);

#endif
    
}


BOOL DeleteAllContact(void);
BOOL AB_IsBeFirst(void);
/*********************************************************************\
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
void AB_MaybeDelete(void)
{
    if(AB_IsBeFirst() == TRUE)
    {
        if(DeleteAllContact() == TRUE)
        {            
            char szOldPath[PATH_MAXLEN];
            int f;
            int first = 0;
            
            szOldPath[0] = 0;
            
            getcwd(szOldPath,PATH_MAXLEN);  
            
            chdir(PATH_DIR_AB);  
            
            f = open("ab.tmp",O_RDWR);
            
            if(f == -1)
            {
                chdir(szOldPath);  
                
                return;
            }
            
            write(f,&first,sizeof(int));
            
            close(f);
            
            chdir(szOldPath);  
        }
    }
}
/*********************************************************************\
* Function	   DeleteAllContact
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
BOOL DeleteAllContact(void)
{
    struct dirent* pData;
    DIR* pDir;
    char szOldPath[PATH_MAXLEN];

    szOldPath[0] = 0;
    
    getcwd(szOldPath,PATH_MAXLEN);  
    chdir(PATH_DIR_AB);  
    pDir = opendir(PATH_DIR_AB);
    if(pDir == NULL)
    {
        closedir(pDir);
        pDir = NULL;
        chdir(szOldPath);
        return FALSE;
    }

    while( (pData = readdir(pDir)) != NULL )
    {
        if(strstr(pData->d_name,PATH_FILE_CONTACT) != NULL 
            || strstr(pData->d_name,".inf") != NULL)
            remove(pData->d_name);
    }
    
    closedir(pDir);
    pDir = NULL;
    chdir(szOldPath);
    return TRUE;
}
/*********************************************************************\
* Function	   IsBeFirst
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
BOOL AB_IsBeFirst(void)
{
    char szOldPath[PATH_MAXLEN];
    int f;
    int first = -1;

    szOldPath[0] = 0;
    
    getcwd(szOldPath,PATH_MAXLEN);  

    chdir(PATH_DIR_AB);  

    f = open("ab.tmp",O_RDONLY);

    if(f == -1)
    {
        f = open("ab.tmp",O_RDWR | O_CREAT,S_IRWXU);

        if(f == -1)
        {
            chdir(szOldPath);  
            
            return FALSE;
        }

        first = 1;

        write(f,&first,sizeof(int));

        close(f);

        chdir(szOldPath);  
        
        return TRUE;
    }

    read(f,&first,sizeof(int));

    close(f);

    chdir(szOldPath);  

    if(first == 0)
        return FALSE;
    else
        return TRUE;    
}
