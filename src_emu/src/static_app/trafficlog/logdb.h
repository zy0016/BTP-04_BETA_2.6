#ifndef _LOGDB_H
#define _LOGDB_H

#include "log.h"

#define  LOGFILEPATH  "/mnt/flash/calllogs/Log.dat"

/* File head structure */
typedef struct logfile_head {
	unsigned long fstrcdtime;
	unsigned long savetime;
} LOG_FILE_HEAD;

/* Record head structure */
typedef struct record_head {
    unsigned long	id;
	RCDTYPE  type;
	DRTTYPE  direction;
	unsigned long	len;	/* length of record including record head*/
	unsigned long   rectime;  
} DB_REC_HEAD;

/* Structure for finding record index and offset */
typedef struct record_index {
    unsigned long id;
    unsigned long offset;
    struct record_index *pNext;
}DB_FIND_INDEX;

/* DB object structure */
typedef struct db{
	FILE * fp;
	unsigned long fstrcdtime;  //first record time
	unsigned long savetime; //save time
	int max_id;  //max id
	int flag;
//	DB_FIND_INDEX *find_list; //s
}LOG_DB;

void* LOG_DB_Open(void);
int LOG_DB_Close(void * log_db);
int LOG_DB_Add(LOG_DB *log_db, const LOGRECORD* log_db_record);
int LOG_DB_Get(void* log_db, unsigned long id, void* record_buf, int record_size);
//int GetDBFlag(LOG_DB *log_db);
BOOL UpdateLogDB(LOG_DB *log_db);
int LOG_DB_Clear(void * log_db);
int GetCurMaxID(LOG_DB *log_db);
int WriteLogTime(LOG_DB *log_db, unsigned long fstrectime, unsigned long savetime);
int GetDBCounter(LOG_DB *log_db);
int GetLogTime(LOG_DB *log_db);
int dayncmp(unsigned long curtime, unsigned long fsttime, unsigned long savetime);
#endif
