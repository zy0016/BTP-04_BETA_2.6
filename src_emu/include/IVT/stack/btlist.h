/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
Module Name:
    btlist.h
Abstract:
	This file includes the definition for thread safe list.
Author:
    Gang He
Revision History:2000.2
---------------------------------------------------------------------------*/

#ifndef BT_LIST_H
#define BT_LIST_H

struct BtList {
	UCHAR *			head;		/* head of the list*/
	UCHAR *			tail;		/* tail of the list*/
	INT16			number;		/* number of members in the list*/
	WORD			len;		/* member data length*/
	SYSTEM_LOCK		lock;		/* lock for the list*/
};
struct NewBtList {
	UCHAR *			head;		/* head of the list*/
	UCHAR *			tail;		/* tail of the list*/
	INT16			number;		/* number of members in the list*/
	WORD			len;		/* member data length*/
};

/*internal functions*/
#ifdef CONFIG_MEMORY_LEAK_DETECT
struct BtList * DBG_BtListNew(WORD len, UCHAR type,char* filename, int line);
UCHAR *	DBG_BtListMemberNew(WORD len, char* filename, int line);
#define BtListNew2(A,T) DBG_BtListNew(A,T, __FILE__, __LINE__)
#else
struct BtList * BtListNew2(WORD len,UCHAR type);
#endif
UCHAR BtListFree2(struct BtList * list,UCHAR type);
void AddHead2(struct BtList * list, UCHAR * data,UCHAR type);
void AddTail2(struct BtList * list, UCHAR * data,UCHAR type);
void InsertAt2(struct BtList * list, UCHAR * pos, UCHAR * data ,UCHAR type);
UCHAR * Find2(struct BtList * list,UCHAR * data, UCHAR len, UCHAR type);
UCHAR * GetHead2(struct BtList * list,UCHAR type);
UCHAR * GetTail2(struct BtList * list,UCHAR type);
UCHAR * RemoveHead2(struct BtList * list,UCHAR type);
UCHAR * RemoveTail2(struct BtList * list,UCHAR type);
UCHAR * RemoveAt2(struct BtList * list, UCHAR * pos,UCHAR type);
void RemoveAll2(struct BtList * list ,UCHAR type);

/*Export functions */
#ifdef CONFIG_MEMORY_LEAK_DETECT
#define BtListMemberNew(A) DBG_BtListMemberNew(A, __FILE__, __LINE__)
#else
UCHAR *	BtListMemberNew(WORD len);
#endif

#define BtListMemberFree FREE
#define Count(list) list->number
#define NEXT(s) (*((ULONG *)s+((list->len+sizeof(ULONG *)-1)>>PTR_BITS)))
				/*				~~~~~~~~~~~~~~~~~~~~~~~~~~~~ end of data, align to 4 bytes*/
#define PREV(s) (*((ULONG *)s+((list->len+sizeof(UCHAR *)+sizeof(ULONG *)-1)>>PTR_BITS)))
#define NEXT_MEM(l,m) (*((ULONG *)m+((l->len+sizeof(ULONG *)-1)>>PTR_BITS)))
#define PREV_MEM(l,m) (*((ULONG *)m+((l->len+sizeof(UCHAR *)+sizeof(ULONG *)-1)>>PTR_BITS)))

UCHAR BtInlist(struct BtList * list, UCHAR * pos);

#define OLD_LIST		0
#define NEW_LIST		1

/*for old list, (with lock)*/
#define BtListNew(l)	BtListNew2(l,OLD_LIST)
#define BtListFree(l)	BtListFree2(l,OLD_LIST)
#define AddHead(l,d)	AddHead2(l,d,OLD_LIST)
#define AddTail(l,d)	AddTail2(l,d,OLD_LIST)
#define InsertAt(l,p,d)	InsertAt2(l,p,d,OLD_LIST)
#define Find(l,d,le)	Find2(l,d,le,OLD_LIST)
#define GetHead(l)		GetHead2(l,OLD_LIST)
#define GetTail(l)		GetTail2(l,OLD_LIST)
#define RemoveHead(l)	RemoveHead2(l,OLD_LIST)
#define RemoveTail(l)	RemoveTail2(l,OLD_LIST)
#define RemoveAt(l,d)	RemoveAt2(l,d,OLD_LIST)
#define RemoveAll(l)	RemoveAll2(l,OLD_LIST)

/*for new list, (no lock)*/
#define BtNewListNew(l)			BtListNew2(l,NEW_LIST)
#define BtNewListFree(l)		BtListFree2(l,NEW_LIST)
#define NewListAddHead(l,d)		AddHead2(l,d,NEW_LIST)
#define NewListAddTail(l,d)		AddTail2(l,d,NEW_LIST)
#define NewListInsertAt(l,p,d)	InsertAt2(l,p,d,NEW_LIST)
#define NewListFind(l,d,le)		Find2(l,d,le,NEW_LIST)
#define NewListGetHead(l)		GetHead2(l,NEW_LIST)
#define NewListGetTail(l)		GetTail2(l,NEW_LIST)
#define NewListRemoveHead(l)	RemoveHead2(l,NEW_LIST)
#define NewListRemoveTail(l)	RemoveTail2(l,NEW_LIST)
#define NewListRemoveAt(l,d)	RemoveAt2(l,d,NEW_LIST)
#define NewListRemoveAll(l)		RemoveAll2(l,NEW_LIST)

#define NewRemoveHead(l)		NewListRemoveHead(l)
#define NewRemoveTail(l)		NewListRemoveTail(l)
#define NewRemoveAt(l,d)		NewListRemoveAt(l,d)
#define NewAddHead(l,d)			NewListAddHead(l,d)
#endif
