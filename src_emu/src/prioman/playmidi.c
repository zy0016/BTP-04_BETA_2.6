/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : playmidi.c
 *
 * Purpose  : 
 *
\**************************************************************************/

#include "plx_pdaex.h"
#include "string.h"
#include "stdio.h"
#include "malloc.h"

#include "prioman.h"
#include "plxmm.h"

#if 0

#define MMRESULT			OS_MMRESULT
#define FOURCC				OS_FOURCC
#define HPSTR				OS_HPSTR
#define HMIDIOUT			OS_HMIDIOUT
#define LPHMIDIOUT			OS_LPHMIDIOUT
#define HMIDISTRM			OS_HMIDISTRM
#define LPHMIDISTRM			OS_LPHMIDISTRM
#define HMMIO				OS_HMMIO
#define HTASK				OS_HTASK
#define MIDIHDR				OS_MIDIHDR
#define LPMIDIHDR			OS_LPMIDIHDR
#define MMIOINFO			OS_MMIOINFO
#define MMCKINFO			OS_MMCKINFO
#define MIDIPROPTIMEDIV		OS_MIDIPROPTIMEDIV
#define MIDIPROPTEMPO		OS_MIDIPROPTEMPO

#define mmioOpen			OS_MmioOpen
#define mmioClose			OS_MmioClose
#define mmioRead			OS_MmioRead
#define mmioWrite			OS_MmioWrite
#define mmioSeek			OS_MmioSeek
#define mmioDescend			OS_MmioDescend
#define mmioAscend			OS_MmioAscend

#define midiStreamOpen		OS_MidiStreamOpen
#define midiStreamClose		OS_MidiStreamClose
#define midiStreamProperty	OS_MidiStreamProperty
#define midiStreamOut		OS_MidiStreamOut
#define midiStreamPause		OS_MidiStreamPause
#define midiStreamRestart	OS_MidiStreamRestart
#define midiStreamStop		OS_MidiStreamStop

#define midiOutOpen				OS_MidiOutOpen
#define midiOutClose			OS_MidiOutClose
#define midiOutPrepareHeader	OS_MidiOutPrepareHeader
#define midiOutUnprepareHeader	OS_MidiOutUnprepareHeader
#define midiOutReset			OS_MidiOutReset

#define GlobalAlloc			OS_GlobalAlloc
#define GlobalReAlloc		OS_GlobalReAlloc
#define GlobalLock			OS_GlobalLock
#define GlobalHandle		OS_GlobalHandle
#define GlobalUnlock		OS_GlobalUnlock
#define GlobalFree			OS_GlobalFree

#define GlobalAllocPtr(flags, cb)   (GlobalLock(GlobalAlloc((flags), (cb))))
#define GlobalReAllocPtr(lp, cbNew, flags)       \
    (GlobalUnlockPtr(lp), GlobalLock(GlobalReAlloc(GlobalPtrHandle(lp) , (cbNew), (flags))))
#define GlobalUnlockPtr(lp)   GlobalUnlock(GlobalPtrHandle(lp))
#define GlobalPtrHandle(lp)   ((HGLOBAL)GlobalHandle(lp))
#define GlobalFreePtr(lp)   (GlobalUnlockPtr(lp), (BOOL)GlobalFree(GlobalPtrHandle(lp)))

#define LocalAlloc			OS_LocalAlloc
#define LocalReAlloc		OS_LocalReAlloc
#define LocalLock			OS_LocalLock
#define LocalHandle			OS_LocalHandle
#define LocalUnlock			OS_LocalUnlock
#define LocalFree			OS_LocalFree

#define assert			OS_Assert
#define MoveMemory		OS_MoveMemory
#define MulDiv			OS_MulDiv
#define Sleep			OS_Sleep

#define SEQ_F_EOF           0x00000001L
#define SEQ_F_COLONIZED     0x00000002L
#define SEQ_F_WAITING       0x00000004L

#define SEQ_S_NOFILE        0
#define SEQ_S_OPENED        1
#define SEQ_S_PREROLLING    2
#define SEQ_S_PREROLLED     3
#define SEQ_S_PLAYING       4
#define SEQ_S_PAUSED        5
#define SEQ_S_STOPPING      6
#define SEQ_S_RESET         7

/*
** Useful macros when dealing with hi-lo format integers
*/
#define DWORDSWAP(dw) \
    ((((dw)>>24)&0x000000FFL)|\
    (((dw)>>8)&0x0000FF00L)|\
    (((dw)<<8)&0x00FF0000L)|\
    (((dw)<<24)&0xFF000000L))

#define WORDSWAP(w) \
    ((((w)>>8)&0x00FF)|\
    (((w)<<8)&0xFF00))

#define MAKEFOURCC(ch0, ch1, ch2, ch3)                              \
		((DWORD)(BYTE)(ch0) | ((DWORD)(BYTE)(ch1) << 8) |   \
		((DWORD)(BYTE)(ch2) << 16) | ((DWORD)(BYTE)(ch3) << 24 ))

#define mmioFOURCC(ch0, ch1, ch2, ch3) MAKEFOURCC(ch0, ch1, ch2, ch3)

#define FOURCC_RMID     mmioFOURCC('R','M','I','D')
#define FOURCC_data     mmioFOURCC('d','a','t','a')
#define FOURCC_MThd     mmioFOURCC('M','T','h','d')
#define FOURCC_MTrk     mmioFOURCC('M','T','r','k')

typedef struct tagCHUNKHDR
{
    FOURCC  fourccType;
    DWORD   dwLength;
}CHUNKHDR, *PCHUNKHDR;

typedef struct tagFILEHDR
{
    WORD    wFormat;
    WORD    wTracks;
    WORD    wDivision;
}FILEHDR, *PFILEHDR;

#define SMF_SUCCESS         (0L)
#define SMF_INVALID_FILE    (1L)
#define SMF_NO_MEMORY       (2L)
#define SMF_OPEN_FAILED     (3L)
#define SMF_INVALID_TRACK   (4L)
#define SMF_META_PENDING    (5L)
#define SMF_ALREADY_OPEN    (6L)
#define SMF_END_OF_TRACK    (7L)
#define SMF_NO_META         (8L)
#define SMF_INVALID_PARM    (9L)
#define SMF_INVALID_BUFFER  (10L)
#define SMF_END_OF_FILE     (11L)
#define SMF_REACHED_TKMAX   (12L)

#define SMF_F_EOF           0x00000001L
#define SMF_F_INSERTSYSEX   0x00000002L

#define SMF_TF_EOT          0x00000001L
#define SMF_TF_INVALID      0x00000002L

#define C_TEMPO_MAP_CHK     16
#define MMSG_DONE           PRIOMAN_MESSAGE_PLAYOVER

/* NOTE: This is arbitrary and only used if there is a tempo map but no
** entry at tick 0.
*/
#define MIDI_DEFAULT_TEMPO      (500000L)

#define MIDI_MSG                ((BYTE)0x80)
#define MIDI_NOTEOFF            ((BYTE)0x80)
#define MIDI_NOTEON             ((BYTE)0x90)
#define MIDI_POLYPRESSURE       ((BYTE)0xA0)
#define MIDI_CONTROLCHANGE      ((BYTE)0xB0)
#define MIDI_PROGRAMCHANGE      ((BYTE)0xC0)
#define MIDI_CHANPRESSURE       ((BYTE)0xD0)
#define MIDI_PITCHBEND          ((BYTE)0xE0)
#define MIDI_META               ((BYTE)0xFF)
#define MIDI_SYSEX              ((BYTE)0xF0)
#define MIDI_SYSEXEND           ((BYTE)0xF7)

#define MIDI_META_TRACKNAME     ((BYTE)0x03)
#define MIDI_META_EOT           ((BYTE)0x2F)
#define MIDI_META_TEMPO         ((BYTE)0x51)
#define MIDI_META_TIMESIG       ((BYTE)0x58)
#define MIDI_META_KEYSIG        ((BYTE)0x59)
#define MIDI_META_SEQSPECIFIC   ((BYTE)0x7F)

#define EVENT_TYPE(event)       ((event).abEvent[0])
#define EVENT_CH_B1(event)      ((event).abEvent[1])
#define EVENT_CH_B2(event)      ((event).abEvent[2])

#define EVENT_META_TYPE(event)  ((event).abEvent[1])

#ifndef _WIN32
#define BCODE      __based(__segname("_CODE"))
#define BSTACK     __based(__segname("_STACK"))
#define BSEG(x)    __based(__segname(x))
#define HUGE       __huge
#else
#define BCODE
#define BSTACK
#define BSEG(x)
#define HUGE
#endif

#define MAX_FILEPATH        256

/* Number and size of playback buffers to keep around
*/
#define C_MIDI_BUFFERS      4
#define CB_MIDI_BUFFERS     1024

char            gszOpenName[MAX_FILEPATH]       = "";
char            gszOpenTitle[MAX_FILEPATH]      = "";
char BCODE      gszFilter[]                     =
    "MIDI File (*.MID;*.RMI)\0*.MID;*.RMI\0"
    "All Files (*.*)\0*.*\0";

char BCODE      gszDefExtension[]               = "MID";

typedef DWORD SMFRESULT;
typedef DWORD TICKS;
typedef BYTE HUGE *HPBYTE;
DECLARE_HANDLE(HSMF);

#define MAX_TICKS               ((TICKS)0xFFFFFFFFL)

typedef struct tagSMFOPENFILESTRUCT
{
    LPSTR       pstrName;
    DWORD       dwTimeDivision;
    HSMF        hSmf;
}SMFOPENFILESTRUCT, *PSMFOPENFILESTRUCT;

typedef struct tagTEMPOMAPENTRY
{
    TICKS       tkTempo;
    DWORD       msBase;
    DWORD       dwTempo;
}TEMPOMAPENTRY, *PTEMPOMAPENTRY;

typedef struct tagSMFFILEINFO
{
    DWORD       dwTracks;
    DWORD       dwFormat;
    DWORD       dwTimeDivision;
    TICKS       tkLength;
}SMFFILEINFO, *PSMFFILEINFO;

typedef struct tagSMF *PSMF;

typedef struct tagTRACK
{
    PSMF        pSmf;
    DWORD       idxTrack;

    TICKS       tkPosition;
    DWORD       cbLeft;
    HPBYTE      hpbImage;
    BYTE        bRunningStatus;
    DWORD       fdwTrack;

    struct
    {
        TICKS   tkLength;
        DWORD   cbLength;
    }smti;
}TRACK, *PTRACK;

typedef struct tagSMF
{
    char        szName[128];
    HPBYTE      hpbImage;
    DWORD       cbImage;
    HTASK       htask;

    TICKS       tkPosition;
    TICKS       tkLength;
    DWORD       dwFormat;
    DWORD       dwTracks;
    DWORD       dwTimeDivision;
    DWORD       fdwSMF;

    DWORD       cTempoMap;
    DWORD       cTempoMapAlloc;
    HLOCAL      hTempoMap;
    PTEMPOMAPENTRY pTempoMap;

    DWORD       dwPendingUserEvent;
    DWORD       cbPendingUserEvent;
    HPBYTE      hpbPendingUserEvent;

    TRACK       rTracks[];
}SMF, *PSMF;

typedef struct tagPREROLL
{
    TICKS       tkBase;
    TICKS       tkEnd;
}PREROLL, FAR *LPPREROLL;

typedef struct tagSEQ *PSEQ;

typedef struct tagSEQ
{
    DWORD       cBuffer;            /* Number of streaming buffers to alloc         */
    DWORD       cbBuffer;           /* Size of each streaming buffer                */
    LPSTR       pstrFile;           /* Pointer to filename to open                  */
    UINT        uDeviceID;          /* Requested MIDI device ID for MMSYSTEM        */
    UINT        uMCIDeviceID;       /* Our MCI device ID given to us                */
    UINT        uMCITimeFormat;     /* Current time format                          */
    UINT        uMCITimeDiv;        /* MCI_SEQ_DIV_xxx for current file             */
	HWND		hWnd;				/* Where to post MMSG_DONE when done playing	*/

    UINT        uState;             /* Sequencer state (SEQ_S_xxx)                  */
    TICKS       tkLength;           /* Length of longest track                      */
    DWORD       cTrk;               /* Number of tracks                             */
    MMRESULT    mmrcLastErr;        /* Error return from last sequencer operation   */

    PSEQ        pNext;              /* Link to next PSEQ                            */
    HSMF        hSmf;               /* Handle to open file                          */
    HMIDIOUT    hmidi;              /* Handle to open MIDI device                   */
    DWORD       dwTimeDivision;     /* File time division                           */

    LPBYTE      lpbAlloc;           /* Streaming buffers -- initial allocation      */
    LPMIDIHDR   lpmhFree;           /* Streaming buffers -- free list               */
    LPMIDIHDR   lpmhPreroll;        /* Streaming buffers -- preroll buffer          */
    DWORD       cbPreroll;          /* Streaming buffers -- size of lpmhPreroll     */
    UINT        uBuffersInMMSYSTEM; /* Streaming buffers -- in use                  */

    TICKS       tkBase;             /* Where playback started from in stream        */
    TICKS       tkEnd;              /* Where playback should end                    */

    DWORD       fdwSeq;             /* Various sequencer flags                      */
}SEQ;

PSEQ gpSeq;

typedef struct tagEVENT
{
    TICKS       tkDelta;
    BYTE        abEvent[3];

    DWORD       cbParm;
    HPBYTE      hpbParm;
}EVENT, BSTACK *SPEVENT;

SMFRESULT smfBuildFileIndex(PSMF BSTACK *ppSmf);
SMFRESULT smfGetNextEvent(PSMF pSmf, EVENT BSTACK *pEvent, TICKS tkMax);
DWORD smfGetVDword(HPBYTE hpbImage, DWORD dwLeft, DWORD BSTACK *pDw);
SMFRESULT smfInsertParmData(PSMF pSmf, TICKS tkDelta, LPMIDIHDR lpmh);

void FAR __stdcall seqMIDICallback(HMIDISTRM hms, UINT uMsg, DWORD dwUser, DWORD dw1, DWORD dw2);
MMRESULT XlatSMFErr(SMFRESULT smfrc);

/*****************************************************************************
*
* smfOpenFile
*
* This function opens a MIDI file for access. 
*
* psofs                     - Specifies the file to open and associated
*                             parameters. Contains a valid HSMF handle
*                             on success.
*
* Returns
*   SMF_SUCCESS The specified file was opened.
*
*   SMF_OPEN_FAILED The specified file could not be opened because it
*     did not exist or could not be created on the disk.
*
*   SMF_INVALID_FILE The specified file was corrupt or not a MIDI file.
* 
*   SMF_NO_MEMORY There was insufficient memory to open the file.
*
*   SMF_INVALID_PARM The given flags or time division in the
*     SMFOPENFILESTRUCT were invalid.
*
*****************************************************************************/
SMFRESULT smfOpenFile(PSMFOPENFILESTRUCT psofs)
{
    HMMIO               hmmio = (HMMIO)NULL;
    PSMF                pSmf;
    SMFRESULT           smfrc = SMF_SUCCESS;
    MMIOINFO            mmioinfo;
    MMCKINFO            ckRIFF;
    MMCKINFO            ckDATA;

    assert(psofs != NULL);
    assert(psofs->pstrName != NULL);

    /* Verify that the file can be opened or created
    */
    memset(&mmioinfo, 0, sizeof(mmioinfo));

    hmmio = mmioOpen(psofs->pstrName, &mmioinfo, MMIO_READ|MMIO_ALLOCBUF);
    if (NULL == hmmio)
    {
        printf("smfOpenFile: mmioOpen failed!\r\n");
        return SMF_OPEN_FAILED;
    }

    /* Now see if we can create the handle structure
    */
    pSmf = (PSMF)LocalAlloc(LPTR, sizeof(SMF));
    if (NULL == pSmf)
    {
        printf("smfOpenFile: LocalAlloc failed!\r\n");
        smfrc = SMF_NO_MEMORY;
        goto smf_Open_File_Cleanup;
    }

    strcpy(pSmf->szName, psofs->pstrName);
    pSmf->fdwSMF = 0;
    pSmf->pTempoMap = NULL;

    /* Pull the entire file into a block of memory
    */
    memset(&ckRIFF, 0, sizeof(ckRIFF));

    if (0 == mmioDescend(hmmio, &ckRIFF, NULL, MMIO_FINDRIFF) &&
        ckRIFF.fccType == FOURCC_RMID)
    {
        ckDATA.ckid = FOURCC_data;

        if (0 == mmioDescend(hmmio, &ckDATA, &ckRIFF, MMIO_FINDCHUNK))
        {
            pSmf->cbImage   = ckDATA.cksize;
        }
        else
        {
            printf("smfOpenFile: Could not descend into RIFF DATA chunk!\r\n");
            smfrc = SMF_INVALID_FILE;
            goto smf_Open_File_Cleanup;
        }
    }
    else
    {
        mmioSeek(hmmio, 0L, SEEK_SET);

        pSmf->cbImage = mmioSeek(hmmio, 0L, SEEK_END);
        mmioSeek(hmmio, 0L, SEEK_SET);
    }

    if (NULL == (pSmf->hpbImage = (HPBYTE)GlobalAllocPtr(GMEM_MOVEABLE|GMEM_SHARE, pSmf->cbImage)))
    {
        printf("smfOpenFile: No memory for image! [%08lX]\r\n", pSmf->cbImage);
        smfrc = SMF_NO_MEMORY;
        goto smf_Open_File_Cleanup;
    }

    if (pSmf->cbImage != (DWORD)mmioRead(hmmio, (HPSTR)pSmf->hpbImage, pSmf->cbImage))
    {
        printf("smfOpenFile: Read error on image!\r\n");
        smfrc = SMF_INVALID_FILE;
        goto smf_Open_File_Cleanup;
    }

    /* If the file exists, parse it just enough to pull out the header and
    ** build a track index.
    */
    smfrc = smfBuildFileIndex((PSMF BSTACK *)&pSmf);
    if (MMSYSERR_NOERROR != smfrc)
    {
        printf("smfOpenFile: smfBuildFileIndex failed! [%lu]\r\n", (DWORD)smfrc);
    }

smf_Open_File_Cleanup:

    mmioClose(hmmio, 0);

    if (SMF_SUCCESS != smfrc)
    {
        if (NULL != pSmf)
        {
            if (NULL != pSmf->hpbImage)
            {
                GlobalFreePtr(pSmf->hpbImage);
            }

            LocalFree((HLOCAL)pSmf);
        }
    }
    else
    {
        psofs->hSmf = (HSMF)pSmf;
    }

    return smfrc;
}

/*****************************************************************************
*
* smfCloseFile
*
* This function closes an open MIDI file.
*
* hSmf                      - The handle of the open file to close.
*
* Returns
*   SMF_SUCCESS The specified file was closed.
*   SMF_INVALID_PARM The given handle was not valid.
*
* Any track handles opened from this file handle are invalid after this
* call.
*
*****************************************************************************/
SMFRESULT smfCloseFile(HSMF hSmf)
{
    PSMF                pSmf = (PSMF)hSmf;

    assert(pSmf != NULL);

    /*
    ** Free up handle memory
    */
    if (NULL != pSmf->hpbImage)
        GlobalFreePtr(pSmf->hpbImage);

    LocalFree((HLOCAL)pSmf);

    return SMF_SUCCESS;
}

/******************************************************************************
*
* smfGetFileInfo This function gets information about the MIDI file.
*
* hSmf                      - Specifies the open MIDI file to inquire about.
*
* psfi                      - A structure which will be filled in with
*                             information about the file.
*
* Returns
*   SMF_SUCCESS Information was gotten about the file.
*   SMF_INVALID_PARM The given handle was invalid.
*
*****************************************************************************/
SMFRESULT smfGetFileInfo(HSMF hSmf, PSMFFILEINFO psfi)
{
    PSMF                pSmf = (PSMF)hSmf;

    assert(pSmf != NULL);
    assert(psfi != NULL);

    /*
    ** Just fill in the structure with useful information.
    */
    psfi->dwTracks      = pSmf->dwTracks;
    psfi->dwFormat      = pSmf->dwFormat;
    psfi->dwTimeDivision= pSmf->dwTimeDivision;
    psfi->tkLength      = pSmf->tkLength;

    return SMF_SUCCESS;
}

/******************************************************************************
*
* smfSeek
*
* This function moves the file pointer within a track
* and gets the state of the track at the new position. It returns a buffer of
* state information which can be used to set up to play from the new position.
*
* hSmf                      - Handle of file to seek within
*
* tkPosition                - The position to seek to in the track.
*
* lpmh                      - A buffer to contain the state information.
*
* Returns
*   SMF_SUCCESS | The state was successfully read.
*   SMF_END_OF_TRACK | The pointer was moved to end of track and no state
*     information was returned.
*   SMF_INVALID_PARM | The given handle or buffer was invalid.
*   SMF_NO_MEMORY | There was insufficient memory in the given buffer to
*     contain all of the state data.
*
* The state information in the buffer includes patch changes, tempo changes,
* time signature, key signature, 
* and controller information. Only the most recent of these paramters before
* the current position will be stored. The state buffer will be returned
* in polymsg format so that it may be directly transmitted over the MIDI
* bus to bring the state up to date.
*
* The buffer is mean to be sent as a streaming buffer; i.e. immediately
* followed by the first data buffer. If the requested tick position
* does not exist in the file, the last event in the buffer
* will be a MEVT_NOP with a delta time calculated to make sure that
* the next stream event plays at the proper time.
*
* The meta events (tempo, time signature, key signature) will be the
* first events in the buffer if they exist.
* 
* Use smfGetStateMaxSize to determine the maximum size of the state
* information buffer. State information that will not fit into the given
* buffer will be lost.
*
* On return, the dwBytesRecorded field of lpmh will contain the
* actual number of bytes stored in the buffer.
*
*****************************************************************************/

typedef struct tagKEYFRAME
{
    /*
    ** Meta events. All FF's indicates never seen.
    */
    BYTE        rbTempo[3];

    /*
    ** MIDI channel messages. FF indicates never seen.
    */
    BYTE        rbProgram[16];
    BYTE        rbControl[16*120];
}KEYFRAME, *PKEYFRAME;

#define KF_EMPTY ((BYTE)0xFF)

SMFRESULT smfSeek(HSMF hSmf, TICKS tkPosition, LPMIDIHDR lpmh)
{
    PSMF                pSmf = (PSMF)hSmf;
    PTRACK              ptrk;
    DWORD               idxTrack;
    SMFRESULT           smfrc;
    EVENT               event;
    LPDWORD             lpdw;
    BYTE                bEvent;
    UINT                idx;
    UINT                idxChannel;
    UINT                idxController;

    static KEYFRAME     kf;

    memset(&kf, 0xFF, sizeof(kf));

    pSmf->tkPosition = 0;
    pSmf->fdwSMF &= ~SMF_F_EOF;

    for (ptrk = pSmf->rTracks, idxTrack = pSmf->dwTracks; idxTrack--; ptrk++)
    {
        ptrk->pSmf              = pSmf;
        ptrk->tkPosition        = 0;
        ptrk->cbLeft            = ptrk->smti.cbLength;
        ptrk->hpbImage          = pSmf->hpbImage + ptrk->idxTrack;
        ptrk->bRunningStatus    = 0;
        ptrk->fdwTrack          = 0;
    }

    while (SMF_SUCCESS == (smfrc = smfGetNextEvent(pSmf, (SPEVENT)&event, tkPosition)))
    {
        if (MIDI_META == (bEvent = EVENT_TYPE(event)))
        {
            if (EVENT_META_TYPE(event) == MIDI_META_TEMPO)
            {
                if (event.cbParm != sizeof(kf.rbTempo))
                    return SMF_INVALID_FILE;

                MoveMemory((HPBYTE)kf.rbTempo, event.hpbParm, event.cbParm);
            }
        }
        else switch(bEvent & 0xF0)
        {
            case MIDI_PROGRAMCHANGE:
                kf.rbProgram[bEvent & 0x0F] = EVENT_CH_B1(event);
                break;

            case MIDI_CONTROLCHANGE:
                kf.rbControl[(((WORD)bEvent & 0x0F)*120) + EVENT_CH_B1(event)] =
                    EVENT_CH_B2(event);
                break;
        }
    }

    if (SMF_REACHED_TKMAX != smfrc)
    {
        return smfrc;
    }

    /* Build lpmh from keyframe
    */
    lpmh->dwBytesRecorded = 0;
    lpdw = (LPDWORD)lpmh->lpData;

    /* Tempo change event?
    */
    if (KF_EMPTY != kf.rbTempo[0] ||
        KF_EMPTY != kf.rbTempo[1] ||
        KF_EMPTY != kf.rbTempo[2])
    {
        if (lpmh->dwBufferLength - lpmh->dwBytesRecorded < 3*sizeof(DWORD))
            return SMF_NO_MEMORY;

        *lpdw++ = 0;
        *lpdw++ = 0;
        *lpdw++ = (((DWORD)kf.rbTempo[0])<<16)|
                  (((DWORD)kf.rbTempo[1])<<8)|
                  ((DWORD)kf.rbTempo[2])|
                  (((DWORD)MEVT_TEMPO) << 24);

        lpmh->dwBytesRecorded += 3*sizeof(DWORD);
    }

    /* Program change events?
    */
    for (idx = 0; idx < 16; idx++)
    {
        if (KF_EMPTY != kf.rbProgram[idx])
        {
            if (lpmh->dwBufferLength - lpmh->dwBytesRecorded < 3*sizeof(DWORD))
                return SMF_NO_MEMORY;

            *lpdw++ = 0;
            *lpdw++ = 0;
            *lpdw++ = (((DWORD)MEVT_SHORTMSG) << 24)      |
                      ((DWORD)MIDI_PROGRAMCHANGE)         |
                      ((DWORD)idx)                        |
                      (((DWORD)kf.rbProgram[idx]) << 8);

            lpmh->dwBytesRecorded += 3*sizeof(DWORD);
        }
    }

    /* Controller events?
    */
    idx = 0;
    for (idxChannel = 0; idxChannel < 16; idxChannel++)
    {
        for (idxController = 0; idxController < 120; idxController++)
        {
            if (KF_EMPTY != kf.rbControl[idx])
            {
                if (lpmh->dwBufferLength - lpmh->dwBytesRecorded < 3*sizeof(DWORD))
                    return SMF_NO_MEMORY;

                *lpdw++ = 0;
                *lpdw++ = 0;
                *lpdw++ = (((DWORD)MEVT_SHORTMSG << 24)     |
                          ((DWORD)MIDI_CONTROLCHANGE)       |
                          ((DWORD)idxChannel)               |
                          (((DWORD)idxController) << 8)     |
                          (((DWORD)kf.rbControl[idx]) << 16));

                lpmh->dwBytesRecorded += 3*sizeof(DWORD);
            }

            idx++;
        }
    }

    /* Force all tracks to be at tkPosition. We are guaranteed that
    ** all tracks will be past the event immediately preceding tkPosition;
    ** this will force correct delta-ticks to be generated so that events
    ** on all tracks will line up properly on a seek into the middle of the
    ** file.
    */
    for (ptrk = pSmf->rTracks, idxTrack = pSmf->dwTracks; idxTrack--; ptrk++)
    {
        ptrk->tkPosition        = tkPosition;
    }

    return SMF_SUCCESS;
}

/******************************************************************************
*
* smfReadEvents
*
* This function reads events from a track.
*
* hSmf                      - Specifies the file to read data from.
*
* lpmh                      - Contains information about the buffer to fill.
*
* tkMax                     - Specifies a cutoff point in the stream
*                             beyond which events will not be read.        
*
* Return@rdes
*   SMF_SUCCESS The events were successfully read.
*   SMF_END_OF_TRACK There are no more events to read in this track.
*   SMF_INVALID_FILE A disk error occured on the file.
*
* @xref <f smfWriteEvents>
*****************************************************************************/
SMFRESULT smfReadEvents(HSMF hSmf, LPMIDIHDR lpmh, TICKS tkMax)
{
    PSMF                pSmf = (PSMF)hSmf;
    SMFRESULT           smfrc;
    EVENT               event;
    LPDWORD             lpdw;
    DWORD               dwTempo;

    assert(pSmf != NULL);
    assert(lpmh != NULL);

    /*
    ** Read events from the track and pack them into the buffer in polymsg
    ** format.
    **
    ** If a SysEx or meta would go over a buffer boundry, split it.
    */
    lpmh->dwBytesRecorded = 0;
    if (pSmf->dwPendingUserEvent)
    {
        smfrc = smfInsertParmData(pSmf, (TICKS)0, lpmh);
        if (SMF_SUCCESS != smfrc)
        {
            printf("smfInsertParmData() -> %u\r\n", (UINT)smfrc);
            return smfrc;
        }
    }

    lpdw = (LPDWORD)(lpmh->lpData + lpmh->dwBytesRecorded);

    if (pSmf->fdwSMF & SMF_F_EOF)
    {
        return SMF_END_OF_FILE;
    }

    while (TRUE)
    {
        assert(lpmh->dwBytesRecorded <= lpmh->dwBufferLength);

        /* If we know ahead of time we won't have room for the
        ** event, just break out now. We need 2 DWORD's for the
        ** terminator event and at least 2 DWORD's for any
        ** event we might store - this will allow us a full
        ** short event or the delta time and stub for a long
        ** event to be split.
        */
        if (lpmh->dwBufferLength - lpmh->dwBytesRecorded < 4*sizeof(DWORD))
        {
            break;
        }

        smfrc = smfGetNextEvent(pSmf, (SPEVENT)&event, tkMax);
        if (SMF_SUCCESS != smfrc)
        {
            /* smfGetNextEvent doesn't set this because smfSeek uses it
            ** as well and needs to distinguish between reaching the
            ** seek point and reaching end-of-file.
            **
            ** To the user, however, we present the selection between
            ** their given tkBase and tkEnd as the entire file, therefore
            ** we want to translate this into EOF.
            */
            if (SMF_REACHED_TKMAX == smfrc)
            {
                pSmf->fdwSMF |= SMF_F_EOF;
            }

            printf("smfReadEvents: smfGetNextEvent() -> %u\r\n", (UINT)smfrc);
            break;
        }

        if (MIDI_SYSEX > EVENT_TYPE(event))
        {
            *lpdw++ = (DWORD)event.tkDelta;
            *lpdw++ = 0;
            *lpdw++ = (((DWORD)MEVT_SHORTMSG)<<24) |
                      ((DWORD)EVENT_TYPE(event)) |
                      (((DWORD)EVENT_CH_B1(event)) << 8) |
                      (((DWORD)EVENT_CH_B2(event)) << 16);

            lpmh->dwBytesRecorded += 3*sizeof(DWORD);
        }
        else if (MIDI_META == EVENT_TYPE(event) &&
                 MIDI_META_EOT == EVENT_META_TYPE(event))
        {
            /* These are ignoreable since smfReadNextEvent()
            ** takes care of track merging
            */
        }
        else if (MIDI_META == EVENT_TYPE(event) &&
                 MIDI_META_TEMPO == EVENT_META_TYPE(event))
        {
            if (event.cbParm != 3)
            {
                printf("smfReadEvents: Corrupt tempo event\r\n");
                return SMF_INVALID_FILE;
            }

            dwTempo = (((DWORD)MEVT_TEMPO)<<24)|
                      (((DWORD)event.hpbParm[0])<<16)|
                      (((DWORD)event.hpbParm[1])<<8)|
                      ((DWORD)event.hpbParm[2]);

            *lpdw++ = (DWORD)event.tkDelta;
            *lpdw++ = 0;
            *lpdw++ = dwTempo;

            lpmh->dwBytesRecorded += 3*sizeof(DWORD);
        }
        else if (MIDI_META != EVENT_TYPE(event))
        {
            /* Must be F0 or F7 system exclusive or FF meta
            ** that we didn't recognize
            */
            pSmf->cbPendingUserEvent = event.cbParm;
            pSmf->hpbPendingUserEvent = event.hpbParm;
            pSmf->fdwSMF &= ~SMF_F_INSERTSYSEX;

            switch (EVENT_TYPE(event))
            {
                case MIDI_SYSEX:
                    pSmf->fdwSMF |= SMF_F_INSERTSYSEX;
            
                    ++pSmf->cbPendingUserEvent;

                    /* Falling through...
                    */

                case MIDI_SYSEXEND:
                    pSmf->dwPendingUserEvent = ((DWORD)MEVT_LONGMSG) << 24;
                    break;
            }

            smfrc = smfInsertParmData(pSmf, event.tkDelta, lpmh);
            if (SMF_SUCCESS != smfrc)
            {
                printf("smfInsertParmData[2] %u\r\n", (UINT)smfrc);
                return smfrc;
            }

            lpdw = (LPDWORD)(lpmh->lpData + lpmh->dwBytesRecorded);
        }
    }

    return (pSmf->fdwSMF & SMF_F_EOF) ? SMF_END_OF_FILE : SMF_SUCCESS;
}

UINT grbChanMsgLen[] =
{
    0,                      /* 0x   not a status byte   */
    0,                      /* 1x   not a status byte   */
    0,                      /* 2x   not a status byte   */
    0,                      /* 3x   not a status byte   */
    0,                      /* 4x   not a status byte   */
    0,                      /* 5x   not a status byte   */
    0,                      /* 6x   not a status byte   */
    0,                      /* 7x   not a status byte   */
    3,                      /* 8x   Note off            */
    3,                      /* 9x   Note on             */
    3,                      /* Ax   Poly pressure       */
    3,                      /* Bx   Control change      */
    2,                      /* Cx   Program change      */
    2,                      /* Dx   Chan pressure       */
    3,                      /* Ex   Pitch bend change   */
    0,                      /* Fx   SysEx (see below)   */
};

/******************************************************************************
*
* smfBuildFileIndex
*
* Preliminary parsing of a MIDI file.
*
* ppSmf                     - Pointer to a returned SMF structure if the
*                             file is successfully parsed.
*
* Returns
*   SMF_SUCCESS The events were successfully read.
*   SMF_NO_MEMORY Out of memory to build key frames.
*   SMF_INVALID_FILE A disk or parse error occured on the file.
*
* This function validates the format of and existing MIDI or RMI file
* and builds the handle structure which will refer to it for the
* lifetime of the instance.
*
* The file header information will be read and verified, and
* smfBuildTrackIndices will be called on every existing track
* to build keyframes and validate the track format.
*
*****************************************************************************/
SMFRESULT smfBuildFileIndex(PSMF BSTACK *ppSmf)
{
    SMFRESULT           smfrc;
    CHUNKHDR *			pCh;
    FILEHDR FAR *       pFh;
    DWORD               idx;
    PSMF                pSmf,
                        pSmfTemp;
    PTRACK              pTrk;
    WORD                wMemory;
    DWORD               dwLeft;
    HPBYTE              hpbImage;

    DWORD               idxTrack;
    EVENT               event;
    BOOL                fFirst;
    DWORD               dwLength;
    HLOCAL              hLocal;
    PTEMPOMAPENTRY      pTempo;

    assert(ppSmf != NULL);

    pSmf = *ppSmf;

    assert(pSmf != NULL);

    /* MIDI data image is already in hpbImage (already extracted from
    ** RIFF header if necessary).
    */

    /* Validate MIDI header
    */
    dwLeft   = pSmf->cbImage;
    hpbImage = pSmf->hpbImage;

    if (dwLeft < sizeof(CHUNKHDR))
        return SMF_INVALID_FILE;

    pCh = (CHUNKHDR FAR *)hpbImage;

    dwLeft   -= sizeof(CHUNKHDR);
    hpbImage += sizeof(CHUNKHDR);

    if (pCh->fourccType != FOURCC_MThd)
        return SMF_INVALID_FILE;

    dwLength = DWORDSWAP(pCh->dwLength);
    if (dwLength < sizeof(FILEHDR) || dwLength > dwLeft)
        return SMF_INVALID_FILE;

    pFh = (FILEHDR FAR *)hpbImage;

    dwLeft   -= dwLength;
    hpbImage += dwLength;

    pSmf->dwFormat       = (DWORD)(WORDSWAP(pFh->wFormat));
    pSmf->dwTracks       = (DWORD)(WORDSWAP(pFh->wTracks));
    pSmf->dwTimeDivision = (DWORD)(WORDSWAP(pFh->wDivision));

    /*
    ** We've successfully parsed the header. Now try to build the track
    ** index.
    **
    ** We only check out the track header chunk here; the track will be
    ** preparsed after we do a quick integretiy check.
    */
    wMemory = sizeof(SMF) + (WORD)(pSmf->dwTracks*sizeof(TRACK));
    pSmfTemp = (PSMF)LocalReAlloc((HLOCAL)pSmf, wMemory, LMEM_MOVEABLE|LMEM_ZEROINIT);

    if (NULL == pSmfTemp)
    {
        printf("No memory for extended pSmf\r\n");
        return SMF_NO_MEMORY;
    }

    pSmf = *ppSmf = pSmfTemp;
    pTrk = pSmf->rTracks;

    for (idx=0; idx<pSmf->dwTracks; idx++)
    {
        if (dwLeft < sizeof(CHUNKHDR))
            return SMF_INVALID_FILE;

        pCh = (CHUNKHDR FAR *)hpbImage;

        dwLeft   -= sizeof(CHUNKHDR);
        hpbImage += sizeof(CHUNKHDR);

        if (pCh->fourccType != FOURCC_MTrk)
            return SMF_INVALID_FILE;

        pTrk->idxTrack      = (DWORD)(hpbImage - pSmf->hpbImage);
        pTrk->smti.cbLength = DWORDSWAP(pCh->dwLength);

        if (pTrk->smti.cbLength > dwLeft)
        {
            printf("Track longer than file!\r\n");
            return SMF_INVALID_FILE;
        }

        dwLeft   -= pTrk->smti.cbLength;
        hpbImage += pTrk->smti.cbLength;

        pTrk++;
    }

    /* File looks OK. Now preparse, doing the following:
    ** (1) Build tempo map so we can convert to/from ticks quickly
    ** (2) Determine actual tick length of file
    ** (3) Validate all events in all tracks
    */
    pSmf->tkPosition = 0;
    pSmf->fdwSMF &= ~SMF_F_EOF;

    for (pTrk = pSmf->rTracks, idxTrack = pSmf->dwTracks; idxTrack--; pTrk++)
    {
        pTrk->pSmf              = pSmf;
        pTrk->tkPosition        = 0;
        pTrk->cbLeft            = pTrk->smti.cbLength;
        pTrk->hpbImage          = pSmf->hpbImage + pTrk->idxTrack;
        pTrk->bRunningStatus    = 0;
        pTrk->fdwTrack          = 0;
    }

    while (SMF_SUCCESS == (smfrc = smfGetNextEvent(pSmf, (EVENT BSTACK *)&event, MAX_TICKS)))
    {
        if (MIDI_META == event.abEvent[0] &&
            MIDI_META_TEMPO == event.abEvent[1])
        {
            if (3 != event.cbParm)
            {
                return SMF_INVALID_FILE;
            }

            if (pSmf->cTempoMap == pSmf->cTempoMapAlloc)
            {
                if (NULL != pSmf->hTempoMap)
                {
                    LocalUnlock(pSmf->hTempoMap);
                }

                pSmf->cTempoMapAlloc += C_TEMPO_MAP_CHK;
                fFirst = FALSE;
                if (0 == pSmf->cTempoMap)
                {
                    hLocal = LocalAlloc(LHND, (UINT)(pSmf->cTempoMapAlloc*sizeof(TEMPOMAPENTRY)));
                    fFirst = TRUE;
                }
                else
                {
                    hLocal = LocalReAlloc(pSmf->hTempoMap, (UINT)(pSmf->cTempoMapAlloc*sizeof(TEMPOMAPENTRY)), LHND);
                }

                if (NULL == hLocal)
                {
                    return SMF_NO_MEMORY;
                }

                pSmf->pTempoMap = (PTEMPOMAPENTRY)LocalLock(pSmf->hTempoMap = hLocal);
            }

            if (fFirst && pSmf->tkPosition != 0)
            {
                /* Inserting first event and the absolute time is zero.
                ** Use defaults of 500,000 uSec/qn from MIDI spec
                */
                pTempo = &pSmf->pTempoMap[pSmf->cTempoMap++];

                pTempo->tkTempo = 0;
                pTempo->msBase  = 0;
                pTempo->dwTempo = MIDI_DEFAULT_TEMPO;

                fFirst = FALSE;
            }

            pTempo = &pSmf->pTempoMap[pSmf->cTempoMap++];

            pTempo->tkTempo = pSmf->tkPosition;
            if (fFirst)
                pTempo->msBase = 0;
            else
            {
                /* NOTE: Better not be here unless we're q/n format!
                */
                pTempo->msBase = (pTempo-1)->msBase +
                                 MulDiv(pTempo->tkTempo-((pTempo-1)->tkTempo),
                                          (pTempo-1)->dwTempo,
                                          1000L*pSmf->dwTimeDivision);
            }
            pTempo->dwTempo = (((DWORD)event.hpbParm[0])<<16)|
                              (((DWORD)event.hpbParm[1])<<8)|
                              ((DWORD)event.hpbParm[2]);
        }
    }

	if (0 == pSmf->cTempoMap)
	{
		printf("File contains no tempo map! Insert default tempo.\r\n");

		hLocal = LocalAlloc(LHND, sizeof(TEMPOMAPENTRY));
		if (!hLocal)
			return SMF_NO_MEMORY;

        pSmf->pTempoMap = (PTEMPOMAPENTRY)LocalLock(pSmf->hTempoMap = hLocal);
		pSmf->cTempoMap = 1;
		pSmf->cTempoMapAlloc = 1;

		pSmf->pTempoMap->tkTempo = 0;
        pSmf->pTempoMap->msBase  = 0;
        pSmf->pTempoMap->dwTempo = MIDI_DEFAULT_TEMPO;
	}

    if (SMF_END_OF_FILE == smfrc || SMF_SUCCESS == smfrc)
    {
        pSmf->tkLength = pSmf->tkPosition;
        smfrc = SMF_SUCCESS;
    }

    return smfrc;
}

/******************************************************************************
*
* smfGetNextEvent
*
* Read the next event from the given file.
*
* pSmf                      - File to read the event from.
*
* pEvent                    - Pointer to an event structure which will receive
*                             basic information about the event.
*
* tkMax                     - Tick destination. An attempt to read past this
*                             position in the file will fail.
*
* Returns
*   SMF_SUCCESS The events were successfully read.
*   SMF_END_OF_FILE There are no more events to read in this track.
*   SMF_REACHED_TKMAX No event was read because <p tkMax> was reached.
*   SMF_INVALID_FILE A disk or parse error occured on the file.
*
* This is the lowest level of parsing for a raw MIDI stream. The basic
* information about one event in the file will be returned in pEvent.
*
* Merging data from all tracks into one stream is performed here.
* 
* pEvent->tkDelta will contain the tick delta for the event.
*
* pEvent->abEvent will contain a description of the event.
*  pevent->abEvent[0] will contain
*    F0 or F7 for a System Exclusive message.
*    FF for a MIDI file meta event.
*    The status byte of any other MIDI message. (Running status will
*    be tracked and expanded).
*
* pEvent->cbParm will contain the number of bytes of paramter data
*   which is still in the file behind the event header already read.
*   This data may be read with <f smfGetTrackEventData>. Any unread
*   data will be skipped on the next call to <f smfGetNextTrackEvent>.
*
* Channel messages (0x8? - 0xE?) will always be returned fully in
*   pevent->abEvent.
*
*  Meta events will contain the meta type in pevent->abEvent[1].
*
*  System exclusive events will contain only an 0xF0 or 0xF7 in
*    pevent->abEvent[0].
*
*  The following fields in pTrk are used to maintain state and must
*  be updated if a seek-in-track is performed:
*
*  bRunningStatus contains the last running status message or 0 if
*   there is no valid running status.
*
*  hpbImage is a pointer into the file image of the first byte of
*   the event to follow the event just read.
*
*  dwLeft contains the number of bytes from hpbImage to the end
*   of the track.
*
* Get the next due event from all (in-use?) tracks
*
* For all tracks
*  If not end-of-track
*   decode event delta time without advancing through buffer
*   event_absolute_time = track_tick_time + track_event_delta_time
*   relative_time = event_absolute_time - last_stream_time
*   if relative_time is lowest so far
*    save this track as the next to pull from, along with times
*
* If we found a track with a due event
*  Advance track pointer past event, saving ptr to parm data if needed
*  track_tick_time += track_event_delta_time
*  last_stream_time = track_tick_time
* Else
*  Mark and return end_of_file
*
*****************************************************************************/
SMFRESULT smfGetNextEvent(PSMF pSmf, EVENT BSTACK *pEvent, TICKS tkMax)
{
    PTRACK              pTrk;
    PTRACK              pTrkFound;
    DWORD               idxTrack;
    TICKS               tkEventDelta;
    TICKS               tkRelTime;
    TICKS               tkMinRelTime;
    BYTE                bEvent;
    DWORD               dwGotTotal;
    DWORD               dwGot;
    DWORD               cbEvent;

    assert(pSmf != NULL);
    assert(pEvent != NULL);

    if (pSmf->fdwSMF & SMF_F_EOF)
    {
        return SMF_END_OF_FILE;
    }

    pTrkFound       = NULL;
    tkMinRelTime    = MAX_TICKS;

    for (pTrk = pSmf->rTracks, idxTrack = pSmf->dwTracks; idxTrack--; pTrk++)
    {
        if (pTrk->fdwTrack & SMF_TF_EOT)
            continue;

        if (!smfGetVDword(pTrk->hpbImage, pTrk->cbLeft, (DWORD BSTACK *)&tkEventDelta))
        {
            printf("Hit end of track w/o end marker!\r\n");
            return SMF_INVALID_FILE;
        }

        tkRelTime = pTrk->tkPosition + tkEventDelta - pSmf->tkPosition;

        if (tkRelTime < tkMinRelTime)
        {
            tkMinRelTime = tkRelTime;
            pTrkFound = pTrk;
        }
    }

    if (!pTrkFound)
    {
        pSmf->fdwSMF |= SMF_F_EOF;
        return SMF_END_OF_FILE;
    }

    pTrk = pTrkFound;

    if (pSmf->tkPosition + tkMinRelTime >= tkMax)
    {
        return SMF_REACHED_TKMAX;
    }

    pTrk->hpbImage += (dwGot = smfGetVDword(pTrk->hpbImage, pTrk->cbLeft, (DWORD BSTACK *)&tkEventDelta));
    pTrk->cbLeft   -= dwGot;

    /* We MUST have at least three bytes here (cause we haven't hit
    ** the end-of-track meta yet, which is three bytes long). Checking
    ** against three means we don't have to check how much is left
    ** in the track again for any short event, which is most cases.
    */
    if (pTrk->cbLeft < 3)
    {
        return SMF_INVALID_FILE;
    }

    pTrk->tkPosition += tkEventDelta;
    pEvent->tkDelta = pTrk->tkPosition - pSmf->tkPosition;
    pSmf->tkPosition = pTrk->tkPosition;

    bEvent = *pTrk->hpbImage++;

    if (MIDI_MSG > bEvent)
    {
        if (0 == pTrk->bRunningStatus)
        {
            return SMF_INVALID_FILE;
        }

        dwGotTotal = 1;
        pEvent->abEvent[0] = pTrk->bRunningStatus;
        pEvent->abEvent[1] = bEvent;
        if (3 == grbChanMsgLen[(pTrk->bRunningStatus >> 4) & 0x0F])
        {
            pEvent->abEvent[2] = *pTrk->hpbImage++;
            dwGotTotal++;
        }
    }
    else if (MIDI_SYSEX > bEvent)
    {
        pTrk->bRunningStatus = bEvent;

        dwGotTotal = 2;
        pEvent->abEvent[0] = bEvent;
        pEvent->abEvent[1] = *pTrk->hpbImage++;
        if (3 == grbChanMsgLen[(bEvent >> 4) & 0x0F])
        {
            pEvent->abEvent[2] = *pTrk->hpbImage++;
            dwGotTotal++;
        }
    }
    else
    {
        pTrk->bRunningStatus = 0;
        if (MIDI_META == bEvent)
        {
            pEvent->abEvent[0] = MIDI_META;
            if (MIDI_META_EOT == (pEvent->abEvent[1] = *pTrk->hpbImage++))
            {
                pTrk->fdwTrack |= SMF_TF_EOT;
            }

            dwGotTotal = 2;
        }
        else if (MIDI_SYSEX == bEvent || MIDI_SYSEXEND == bEvent)
        {
            pEvent->abEvent[0] = bEvent;
            dwGotTotal = 1;
        }
        else
        {
            return SMF_INVALID_FILE;
        }

        if (0 == (dwGot = smfGetVDword(pTrk->hpbImage, pTrk->cbLeft - 2, (DWORD BSTACK *)&cbEvent)))
        {
            return SMF_INVALID_FILE;
        }

        pTrk->hpbImage  += dwGot;
        dwGotTotal      += dwGot;

        if (dwGotTotal + cbEvent > pTrk->cbLeft)
        {
            return SMF_INVALID_FILE;
        }

        pEvent->cbParm  = cbEvent;
        pEvent->hpbParm = pTrk->hpbImage;

        pTrk->hpbImage += cbEvent;
        dwGotTotal     += cbEvent;
    }

    assert(pTrk->cbLeft >= dwGotTotal);

    pTrk->cbLeft -= dwGotTotal;

    return SMF_SUCCESS;
}

/******************************************************************************
*
* smfGetVDword
*
* Reads a variable length DWORD from the given file.
*
* hpbImage                  - Pointer to the first byte of the VDWORD.
*
* dwLeft                    - Bytes left in image
*
* pDw                       - Pointer to a DWORD to store the result in.
*                             track.
*
* Returns the number of bytes consumed from the stream.
*
* A variable length DWORD stored in a MIDI file contains one or more
* bytes. Each byte except the last has the high bit set; only the
* low 7 bits are significant.
*
*****************************************************************************/
DWORD smfGetVDword(HPBYTE hpbImage, DWORD dwLeft, DWORD BSTACK *pDw)
{
    BYTE                b;
    DWORD               dwUsed  = 0;

    assert(hpbImage != NULL);
    assert(pDw != NULL);

    *pDw = 0;

    do
    {
        if (!dwLeft)
        {
            return 0;
        }

        b = *hpbImage++;
        dwLeft--;
        dwUsed++;

        *pDw = (*pDw << 7) | (b & 0x7F);
    }
	while (b&0x80);

    return dwUsed;
}

/******************************************************************************
*
* smfInsertParmData
*
* Inserts pending long data from a track into the given buffer.
*
* pSmf                      - Specifies the file to read data from.
*
* tkDelta                   - Specfices the tick delta for the data.
*
* lpmh                      - Contains information about the buffer to fill.
*
* Returns
*   SMF_SUCCESS The events were successfully read.
*   SMF_INVALID_FILE A disk error occured on the file.
*
* Fills as much data as will fit while leaving room for the buffer
* terminator.
*
* If the long data is depleted, resets pSmf->dwPendingUserEvent so
* that the next event may be read.
*
*****************************************************************************/
SMFRESULT smfInsertParmData(PSMF pSmf, TICKS tkDelta, LPMIDIHDR lpmh)
{
    DWORD               dwLength;
    DWORD               dwRounded;
    LPDWORD             lpdw;

    assert(pSmf != NULL);
    assert(lpmh != NULL);

    /* Can't fit 4 DWORD's? (tkDelta + stream-id + event + some data)
    ** Can't do anything.
    */
    assert(lpmh->dwBufferLength >= lpmh->dwBytesRecorded);

    if (lpmh->dwBufferLength - lpmh->dwBytesRecorded < 4*sizeof(DWORD))
    {
        if (0 == tkDelta)
            return SMF_SUCCESS;

        /* If we got here with a real delta, that means smfReadEvents screwed
        ** up calculating left space and we should flag it somehow.
        */
        printf("Can't fit initial piece of SysEx into buffer!\r\n");
        return SMF_INVALID_FILE;
    }

    lpdw = (LPDWORD)(lpmh->lpData + lpmh->dwBytesRecorded);

    dwLength = lpmh->dwBufferLength - lpmh->dwBytesRecorded - 3*sizeof(DWORD);
    dwLength = min(dwLength, pSmf->cbPendingUserEvent);

    *lpdw++ = (DWORD)tkDelta;
    *lpdw++ = 0L;
    *lpdw++ = (pSmf->dwPendingUserEvent & 0xFF000000L) | (dwLength & 0x00FFFFFFL);

    dwRounded = (dwLength + 3) & (~3L);

    if (pSmf->fdwSMF & SMF_F_INSERTSYSEX)
    {
        *(LPBYTE)lpdw++ = MIDI_SYSEX;
        pSmf->fdwSMF &= ~SMF_F_INSERTSYSEX;
        --dwLength;
        --pSmf->cbPendingUserEvent;
    }

    if (dwLength & 0x80000000L)
    {
        printf("dwLength %08lX  dwBytesRecorded %08lX  dwBufferLength %08lX\r\n", dwLength, lpmh->dwBytesRecorded, lpmh->dwBufferLength);
        printf("cbPendingUserEvent %08lX  dwPendingUserEvent %08lX dwRounded %08lX\r\n", pSmf->cbPendingUserEvent, pSmf->dwPendingUserEvent, dwRounded);
        printf("Offset into MIDI image %08lX\r\n", (DWORD)(pSmf->hpbPendingUserEvent - pSmf->hpbImage));
        printf("!hmemcpy is about to fault\r\n");
    }

    MoveMemory(lpdw, pSmf->hpbPendingUserEvent, dwLength);
    if (0 == (pSmf->cbPendingUserEvent -= dwLength))
        pSmf->dwPendingUserEvent = 0;

    lpmh->dwBytesRecorded += 3*sizeof(DWORD) + dwRounded;

    return SMF_SUCCESS;
}

/******************************************************************************
*
* smfGetStateMaxSize
*
* This function returns the maximum sizeof buffer that is needed to
* hold the state information returned by f smfSeek.
*
* pdwSize                   - Gets the size in bytes that should be allocated
*                             for the state buffer.
*
* Returns the state size in bytes.
*
*****************************************************************************/
DWORD smfGetStateMaxSize(VOID)
{
    return  3*sizeof(DWORD) +           /* Tempo                */
            3*16*sizeof(DWORD) +        /* Patch changes        */
            3*16*120*sizeof(DWORD) +    /* Controller changes   */
            3*sizeof(DWORD);            /* Time alignment NOP   */
}

/***************************************************************************
*  
* seqAllocBuffers
*
* Allocate buffers for this instance.
*
* pSeq                      - The sequencer instance to allocate buffers for.
*
* Returns
*   MMSYSERR_NOERROR If the operation was successful.
*
*   MCIERR_OUT_OF_MEMORY  If there is insufficient memory for
*     the requested number and size of buffers.
*
* seqAllocBuffers allocates playback buffers based on the
* cbBuffer and cBuffer fields of pSeq. cbBuffer specifies the
* number of bytes in each buffer, and cBuffer specifies the
* number of buffers to allocate.
*
* seqAllocBuffers must be called before any other sequencer call
* on a newly allocted SEQUENCE structure. It must be paired with
* a call to seqFreeBuffers, which should be the last call made
* before the SEQUENCE structure is discarded.
*
***************************************************************************/
MMRESULT seqAllocBuffers(PSEQ pSeq)
{
    DWORD               dwEachBufferSize;
    DWORD               dwAlloc;
    UINT                i;
    LPBYTE              lpbWork;

    assert(pSeq != NULL);

    pSeq->uState    = SEQ_S_NOFILE;
    pSeq->lpmhFree  = NULL;
    pSeq->lpbAlloc  = NULL;
    pSeq->hSmf      = NULL;

    /* First make sure we can allocate the buffers they asked for
    */
    dwEachBufferSize = sizeof(MIDIHDR) + (DWORD)(pSeq->cbBuffer);
    dwAlloc          = dwEachBufferSize * (DWORD)(pSeq->cBuffer);

    pSeq->lpbAlloc = (LPBYTE)GlobalAllocPtr(GMEM_MOVEABLE|GMEM_SHARE, dwAlloc);
    if (NULL == pSeq->lpbAlloc)
        return MCIERR_OUT_OF_MEMORY;

    /* Initialize all MIDIHDR's and throw them into a free list
    */
    pSeq->lpmhFree = NULL;

    lpbWork = pSeq->lpbAlloc;
    for (i=0; i < pSeq->cBuffer; i++)
    {
        ((LPMIDIHDR)lpbWork)->lpNext            = pSeq->lpmhFree;

        ((LPMIDIHDR)lpbWork)->lpData            = (LPSTR)lpbWork + sizeof(MIDIHDR);
        ((LPMIDIHDR)lpbWork)->dwBufferLength    = pSeq->cbBuffer;
        ((LPMIDIHDR)lpbWork)->dwBytesRecorded   = 0;
        ((LPMIDIHDR)lpbWork)->dwUser            = (DWORD)pSeq;
        ((LPMIDIHDR)lpbWork)->dwFlags           = 0;

        pSeq->lpmhFree = (LPMIDIHDR)lpbWork;

        lpbWork += dwEachBufferSize;
    }

    return MMSYSERR_NOERROR;
}

/***************************************************************************
*
* seqFreeBuffers
*
* Free buffers for this instance.
*
* pSeq                      - The sequencer instance to free buffers for.
*
* seqFreeBuffers frees all allocated memory belonging to the
* given sequencer instance pSeq. It must be the last call
* performed on the instance before it is destroyed.
*
****************************************************************************/
VOID seqFreeBuffers(PSEQ pSeq)
{
    LPMIDIHDR           lpmh;

    assert(pSeq != NULL);

    if (NULL != pSeq->lpbAlloc)
    {
        lpmh = (LPMIDIHDR)pSeq->lpbAlloc;
        assert(!(lpmh->dwFlags & MHDR_PREPARED));

        GlobalFreePtr(pSeq->lpbAlloc);
    }
}

/***************************************************************************
*
* seqOpenFile
*
* Associates a MIDI file with the given sequencer instance.
*
* pSeq                      - The sequencer instance.
*
* Returns
*   MMSYSERR_NOERROR If the operation is successful.
*
*   MCIERR_UNSUPPORTED_FUNCTION If there is already a file open
*     on this instance.
*
*   MCIERR_OUT_OF_MEMORY If there was insufficient memory to
*     allocate internal buffers on the file.
*
*   MCIERR_INVALID_FILE If initial attempts to parse the file
*     failed (such as the file is not a MIDI or RMI file).
*
* seqOpenFile may only be called if there is no currently open file
* on the instance. It must be paired with a call to seqCloseFile
* when operations on this file are complete.
*
* The pstrFile field of pSeq contains the name of the file
* to open. This name will be passed directly to mmioOpen; it may
* contain a specifcation for a custom MMIO file handler. The task
* context used for all I/O will be the task which calls seqOpenFile.
*
***************************************************************************/

MMRESULT seqCloseFile(PSEQ pSeq);

MMRESULT seqOpenFile(PSEQ pSeq)
{                            
    MMRESULT            rc = MMSYSERR_NOERROR;
    SMFOPENFILESTRUCT   sofs;
    SMFFILEINFO         sfi;
    SMFRESULT           smfrc;
    DWORD               cbBuffer;

    assert(pSeq != NULL);

    if (pSeq->uState != SEQ_S_NOFILE)
    {
        return MCIERR_UNSUPPORTED_FUNCTION;
    }

    assert(pSeq->pstrFile != NULL);

    sofs.pstrName     = pSeq->pstrFile;

    smfrc = smfOpenFile(&sofs);
    if (SMF_SUCCESS != smfrc)
    {
        rc = XlatSMFErr(smfrc);
        goto Seq_Open_File_Cleanup;
    }

    pSeq->hSmf = sofs.hSmf;
    smfGetFileInfo(pSeq->hSmf, &sfi);

    pSeq->dwTimeDivision = sfi.dwTimeDivision;
    pSeq->tkLength       = sfi.tkLength;
    pSeq->cTrk           = sfi.dwTracks;

    /* Track buffers must be big enough to hold the state data returned
    ** by smfSeek()
    */
    cbBuffer = min(pSeq->cbBuffer, smfGetStateMaxSize());

Seq_Open_File_Cleanup:
    if (MMSYSERR_NOERROR != rc)
        seqCloseFile(pSeq);
    else
        pSeq->uState = SEQ_S_OPENED;

    return rc;
}

/***************************************************************************
*
* seqCloseFile
*
* Deassociates a MIDI file with the given sequencer instance.
*
* pSeq                      -  The sequencer instance.
*
* Returns
*   MMSYSERR_NOERROR If the operation is successful.
*
*   MCIERR_UNSUPPORTED_FUNCTION If the sequencer instance is not
*     stopped.
*
* A call to seqCloseFile must be paired with a prior call to
* seqOpenFile. All buffers associated with the file will be
* freed and the file will be closed. The sequencer must be
* stopped before this call will be accepted.
*
***************************************************************************/
MMRESULT seqCloseFile(PSEQ pSeq)
{
    LPMIDIHDR           lpmh;

    assert(pSeq != NULL);

    if (SEQ_S_OPENED != pSeq->uState)
        return MCIERR_UNSUPPORTED_FUNCTION;

    if ((HSMF)NULL != pSeq->hSmf)
    {
        smfCloseFile(pSeq->hSmf);
        pSeq->hSmf = (HSMF)NULL;
    }

    /* If we were prerolled, need to clean up -- have an open MIDI handle
    ** and buffers in the ready queue
    */
    for (lpmh = pSeq->lpmhFree; lpmh; lpmh = lpmh->lpNext)
        midiOutUnprepareHeader(pSeq->hmidi, lpmh, sizeof(*lpmh));

    if (pSeq->lpmhPreroll)
        midiOutUnprepareHeader(pSeq->hmidi, pSeq->lpmhPreroll, sizeof(*pSeq->lpmhPreroll));

    if (pSeq->hmidi != NULL)
    {
        midiStreamClose((HMIDISTRM)pSeq->hmidi);
        pSeq->hmidi = NULL;
    }

    pSeq->uState = SEQ_S_NOFILE;

    return MMSYSERR_NOERROR;
}

/***************************************************************************
*  
* seqPreroll
*
* Prepares the file for playback at the given position.
*
* pSeq                      - The sequencer instance.
*
* lpPreroll                 - Specifies the starting and ending tick
*                             positions to play between.
*
* Returns
*   MMSYSERR_NOERROR If the operation is successful.
*    
*   MCIERR_UNSUPPORTED_FUNCTION If the sequencer instance is not
*     opened or prerolled.
*
* Open the device so we can initialize channels.
*
* Loop through the tracks. For each track, seek to the given position and
* send the init data SMF gives us to the handle.
*
* Wait for all init buffers to finish.
*
* Unprepare the buffers (they're only ever sent here; the sequencer
* engine merges them into a single stream during normal playback) and
* refill them with the first chunk of data from the track. 
*
****************************************************************************/
MMRESULT seqPreroll(PSEQ pSeq, LPPREROLL lpPreroll)
{
    SMFRESULT           smfrc;
    MMRESULT            mmrc = MMSYSERR_NOERROR;
    MIDIPROPTIMEDIV     mptd;
    LPMIDIHDR           lpmh = NULL;
    LPMIDIHDR           lpmhPreroll = NULL;
    DWORD               cbPrerollBuffer;
    UINT                uDeviceID;

    assert(pSeq != NULL);

    pSeq->mmrcLastErr = MMSYSERR_NOERROR;

    if (pSeq->uState != SEQ_S_OPENED &&
        pSeq->uState != SEQ_S_PREROLLED)
        return MCIERR_UNSUPPORTED_FUNCTION;

	pSeq->tkBase = lpPreroll->tkBase;
	pSeq->tkEnd = lpPreroll->tkEnd;

    if (pSeq->hmidi)
    {
        // Recollect buffers from MMSYSTEM back into free queue
        //
        pSeq->uState = SEQ_S_RESET;
        midiOutReset(pSeq->hmidi);

		while (pSeq->uBuffersInMMSYSTEM)
			Sleep(0);
    }

    pSeq->uBuffersInMMSYSTEM = 0;
    pSeq->uState = SEQ_S_PREROLLING;

    //
    // We've successfully opened the file and all of the tracks; now
    // open the MIDI device and set the time division.
    //
    // NOTE: seqPreroll is equivalent to seek; device might already be open
    //
    if (NULL == pSeq->hmidi)
    {
        uDeviceID = pSeq->uDeviceID;
        if ((mmrc = midiStreamOpen((LPHMIDISTRM)&pSeq->hmidi,
                                   &uDeviceID,
                                   1,
                                   (DWORD)seqMIDICallback,
                                   0,
                                   CALLBACK_FUNCTION)) != MMSYSERR_NOERROR)
        {
            pSeq->hmidi = NULL;
            goto seq_Preroll_Cleanup;
        }

        mptd.cbStruct  = sizeof(mptd);
        mptd.dwTimeDiv = pSeq->dwTimeDivision;
        if ((mmrc = midiStreamProperty((HMIDISTRM)pSeq->hmidi,
                                       (LPBYTE)&mptd,
                                       MIDIPROP_SET|MIDIPROP_TIMEDIV)) != MMSYSERR_NOERROR)
        {
            printf("midiStreamProperty() -> %04X\r\n", (WORD)mmrc);
            midiStreamClose((HMIDISTRM)pSeq->hmidi);
            pSeq->hmidi = NULL;
            mmrc = MCIERR_DEVICE_NOT_READY;
            goto seq_Preroll_Cleanup;
        }
    }

    mmrc = MMSYSERR_NOERROR;

    //
    //  Allocate a preroll buffer.  Then if we don't have enough room for
    //  all the preroll info, we make the buffer larger.
    //
    if (!pSeq->lpmhPreroll)
    {
        cbPrerollBuffer = 4096;
        lpmhPreroll = (LPMIDIHDR)GlobalAllocPtr(GMEM_MOVEABLE|GMEM_SHARE, cbPrerollBuffer);
    }
    else
    {
        cbPrerollBuffer = pSeq->cbPreroll;
        lpmhPreroll = pSeq->lpmhPreroll;
    }

    lpmhPreroll->lpNext            = pSeq->lpmhFree;
    lpmhPreroll->lpData            = (LPSTR)lpmhPreroll + sizeof(MIDIHDR);
    lpmhPreroll->dwBufferLength    = cbPrerollBuffer - sizeof(MIDIHDR);
    lpmhPreroll->dwBytesRecorded   = 0;
    lpmhPreroll->dwUser            = (DWORD)pSeq;
    lpmhPreroll->dwFlags           = 0;

    do
    {
        smfrc = smfSeek(pSeq->hSmf, pSeq->tkBase, lpmhPreroll);
        if (SMF_SUCCESS != smfrc)
        {
            if ((SMF_NO_MEMORY != smfrc) ||
                (cbPrerollBuffer >= 32768L))
            {
                printf("smfSeek() returned %lu\r\n", (DWORD)smfrc);

                GlobalFreePtr(lpmhPreroll);
                pSeq->lpmhPreroll = NULL;

                mmrc = XlatSMFErr(smfrc);
                goto seq_Preroll_Cleanup;
            }
            else   //  Try to grow buffer.
            {
                cbPrerollBuffer *= 2;
                lpmh = (LPMIDIHDR)GlobalReAllocPtr(lpmhPreroll, cbPrerollBuffer, 0);
                if (NULL == lpmh)
                {
                    printf("seqPreroll - realloc failed, aborting preroll.\r\n");
                    mmrc = MCIERR_OUT_OF_MEMORY;
                    goto seq_Preroll_Cleanup;
                }

                lpmhPreroll = lpmh;
                lpmhPreroll->lpData = (LPSTR)lpmhPreroll + sizeof(MIDIHDR);
                lpmhPreroll->dwBufferLength = cbPrerollBuffer - sizeof(MIDIHDR);

                pSeq->lpmhPreroll = lpmhPreroll;
                pSeq->cbPreroll = cbPrerollBuffer;
            }
        }
    }
	while (SMF_SUCCESS != smfrc);

    if (MMSYSERR_NOERROR != (mmrc = midiOutPrepareHeader(pSeq->hmidi, lpmhPreroll, sizeof(MIDIHDR))))
    {
        printf("midiOutPrepare(preroll) -> %lu!\r\n", (DWORD)mmrc);

        mmrc = MCIERR_DEVICE_NOT_READY;
        goto seq_Preroll_Cleanup;
    }

    ++pSeq->uBuffersInMMSYSTEM;

    if (MMSYSERR_NOERROR != (mmrc = midiStreamOut((HMIDISTRM)pSeq->hmidi, lpmhPreroll, sizeof(MIDIHDR))))
    {
        printf("midiStreamOut(preroll) -> %lu!\r\n", (DWORD)mmrc);

        mmrc = MCIERR_DEVICE_NOT_READY;
        --pSeq->uBuffersInMMSYSTEM;
        goto seq_Preroll_Cleanup;
    }
    printf("seqPreroll: midiStreamOut(0x%x,0x%lx,%u) returned %u.\r\n",pSeq->hmidi,lpmhPreroll,sizeof(MIDIHDR),mmrc);

    pSeq->fdwSeq &= ~SEQ_F_EOF;
    while (pSeq->lpmhFree)
    {
        lpmh = pSeq->lpmhFree;
        pSeq->lpmhFree = lpmh->lpNext;

        smfrc = smfReadEvents(pSeq->hSmf, lpmh, pSeq->tkEnd);
        if (SMF_SUCCESS != smfrc && SMF_END_OF_FILE != smfrc)
        {
            printf("SFP: smfReadEvents() -> %u\r\n", (UINT)smfrc);
            mmrc = XlatSMFErr(smfrc);
            goto seq_Preroll_Cleanup;
        }

        if (MMSYSERR_NOERROR != (mmrc = midiOutPrepareHeader(pSeq->hmidi, lpmh, sizeof(*lpmh))))
        {
            printf("SFP: midiOutPrepareHeader failed\r\n");
            goto seq_Preroll_Cleanup;
        }

        if (MMSYSERR_NOERROR != (mmrc = midiStreamOut((HMIDISTRM)pSeq->hmidi, lpmh, sizeof(*lpmh))))
        {
            printf("SFP: midiStreamOut failed\r\n");
            goto seq_Preroll_Cleanup;
        }

        ++pSeq->uBuffersInMMSYSTEM;

        if (SMF_END_OF_FILE == smfrc)
        {
            pSeq->fdwSeq |= SEQ_F_EOF;
            break;
        }
    }

seq_Preroll_Cleanup:
    if (MMSYSERR_NOERROR != mmrc)
    {
        pSeq->uState = SEQ_S_OPENED;
        pSeq->fdwSeq &= ~SEQ_F_WAITING;
    }
    else
    {
        pSeq->uState = SEQ_S_PREROLLED;
    }

    return mmrc;
}

/***************************************************************************
*
* seqStart
*
* Starts playback at the current position.
*
* pSeq                      - The sequencer instance.
*
* Returns
*   MMSYSERR_NOERROR If the operation is successful.
*
*   MCIERR_UNSUPPORTED_FUNCTION If the sequencer instance is not
*     stopped.
*
*   MCIERR_DEVICE_NOT_READY If the underlying MIDI device could
*     not be opened or fails any call.
*
* The sequencer must be prerolled before seqStart may be called.
*
* Just feed everything in the ready queue to the device.
*
***************************************************************************/
MMRESULT seqStart(PSEQ pSeq)
{
    assert(NULL != pSeq);

    if (SEQ_S_PREROLLED != pSeq->uState)
    {
        printf("seqStart(): State is wrong! [%u]\r\n", pSeq->uState);
        return MCIERR_UNSUPPORTED_FUNCTION;
    }

    pSeq->uState = SEQ_S_PLAYING;

    return midiStreamRestart((HMIDISTRM)pSeq->hmidi);
}

/***************************************************************************
*
* seqPause
*
* Pauses playback of the instance.
*
* pSeq                      - The sequencer instance.
*
* Returns
*   MMSYSERR_NOERROR If the operation is successful.
*
*   MCIERR_UNSUPPORTED_FUNCTION If the sequencer instance is not
*     playing.
*
* The sequencer must be playing before seqPause may be called.
* Pausing the sequencer will cause all currently on notes to be turned
* off. This may cause playback to be slightly inaccurate on restart
* due to missing notes.
*
***************************************************************************/
MMRESULT seqPause(PSEQ pSeq)
{
    assert(NULL != pSeq);

    if (SEQ_S_PLAYING != pSeq->uState)
        return MCIERR_UNSUPPORTED_FUNCTION;

    pSeq->uState = SEQ_S_PAUSED;
    midiStreamPause((HMIDISTRM)pSeq->hmidi);

    return MMSYSERR_NOERROR;
}

/***************************************************************************
*
* seqRestart
*
* Restarts playback of an instance after a pause.
*
* pSeq                      - The sequencer instance.
*
* Returns
*    MMSYSERR_NOERROR If the operation is successful.
*
*    MCIERR_UNSUPPORTED_FUNCTION If the sequencer instance is not
*     paused.
*
* The sequencer must be paused before seqRestart may be called.
*
***************************************************************************/
MMRESULT seqRestart(PSEQ pSeq)
{
    assert(NULL != pSeq);

    if (SEQ_S_PAUSED != pSeq->uState)
        return MCIERR_UNSUPPORTED_FUNCTION;

    pSeq->uState = SEQ_S_PLAYING;
    midiStreamRestart((HMIDISTRM)pSeq->hmidi);

    return MMSYSERR_NOERROR;
}

/***************************************************************************
*
* seqStop
*
* Totally stops playback of an instance.
*
* pSeq                      - The sequencer instance.
*
* Returns
*   MMSYSERR_NOERROR If the operation is successful.
*
*   MCIERR_UNSUPPORTED_FUNCTION If the sequencer instance is not
*     paused or playing.
*
* The sequencer must be paused or playing before seqStop may be called.
*
***************************************************************************/
MMRESULT seqStop(PSEQ pSeq)
{
    assert(NULL != pSeq);

    /* Automatic success if we're already stopped
    */
    if (SEQ_S_PLAYING != pSeq->uState &&
        SEQ_S_PAUSED != pSeq->uState)
    {
        pSeq->fdwSeq &= ~SEQ_F_WAITING;
        return MMSYSERR_NOERROR;
    }

    pSeq->uState = SEQ_S_STOPPING;
    pSeq->fdwSeq |= SEQ_F_WAITING;

    if (MMSYSERR_NOERROR != (pSeq->mmrcLastErr = midiStreamStop((HMIDISTRM)pSeq->hmidi)))
    {
        printf("midiOutStop() returned %lu in seqStop()!\r\n", (DWORD)pSeq->mmrcLastErr);

        pSeq->fdwSeq &= ~SEQ_F_WAITING;
        return MCIERR_DEVICE_NOT_READY;
    }

	while (pSeq->uBuffersInMMSYSTEM)
		Sleep(0);

    return MMSYSERR_NOERROR;
}

/***************************************************************************
*
* seqMIDICallback
*
* Called by the system when a buffer is done
*
* dw1                       - The buffer that has completed playback.
*
***************************************************************************/
void FAR __stdcall seqMIDICallback(HMIDISTRM hms, UINT uMsg, DWORD dwUser, DWORD dw1, DWORD dw2)
{
	LPMIDIHDR           lpmh = (LPMIDIHDR)dw1;
    PSEQ                pSeq;
    MMRESULT            mmrc;
    SMFRESULT           smfrc;

	if (uMsg != MOM_DONE)
		return;

	assert(NULL != lpmh);

    pSeq = (PSEQ)(lpmh->dwUser);

    assert(pSeq != NULL);

    --pSeq->uBuffersInMMSYSTEM;

    if (SEQ_S_RESET == pSeq->uState)
    {
        // We're recollecting buffers from MMSYSTEM
        //
		if (lpmh != pSeq->lpmhPreroll)
		{
        	lpmh->lpNext   = pSeq->lpmhFree;
        	pSeq->lpmhFree = lpmh;
		}

        return;
    }

    if ((SEQ_S_STOPPING == pSeq->uState) || (pSeq->fdwSeq & SEQ_F_EOF))
    {
        /*
        ** Reached EOF, just put the buffer back on the free
        ** list
        */
		if (lpmh != pSeq->lpmhPreroll)
		{
        	lpmh->lpNext   = pSeq->lpmhFree;
        	pSeq->lpmhFree = lpmh;
		}

        if (MMSYSERR_NOERROR != (mmrc = midiOutUnprepareHeader(pSeq->hmidi, lpmh, sizeof(*lpmh))))
        {
            printf("midiOutUnprepareHeader failed in seqBufferDone! (%lu)\r\n", (DWORD)mmrc);
        }

        if (0 == pSeq->uBuffersInMMSYSTEM)
        {
            printf("seqBufferDone: normal sequencer shutdown.\r\n");

            /* Totally done! Free device and notify.
            */
            midiStreamClose((HMIDISTRM)pSeq->hmidi);

            pSeq->hmidi = NULL;
            pSeq->uState = SEQ_S_OPENED;
            pSeq->mmrcLastErr = MMSYSERR_NOERROR;
            pSeq->fdwSeq &= ~SEQ_F_WAITING;

        	// lParam indicates whether or not to preroll again. Don't if we were explicitly
        	// stopped.
        	//
            PostMessage(pSeq->hWnd, MMSG_DONE, (WPARAM)pSeq, (LPARAM)(SEQ_S_STOPPING != pSeq->uState));
        }
    }
    else
    {
        /*
        ** Not EOF yet; attempt to fill another buffer
        */
        smfrc = smfReadEvents(pSeq->hSmf, lpmh, pSeq->tkEnd);

        switch (smfrc)
        {
            case SMF_SUCCESS:
                break;

            case SMF_END_OF_FILE:
                pSeq->fdwSeq |= SEQ_F_EOF;
                smfrc = SMF_SUCCESS;
                break;

            default:
                printf("smfReadEvents returned %lu in callback!\r\n", (DWORD)smfrc);
                pSeq->uState = SEQ_S_STOPPING;
                break;
        }

        if (SMF_SUCCESS == smfrc)
        {
            ++pSeq->uBuffersInMMSYSTEM;
            mmrc = midiStreamOut((HMIDISTRM)pSeq->hmidi, lpmh, sizeof(*lpmh));
            if (MMSYSERR_NOERROR != mmrc)
            {
                printf("seqBufferDone(): midiStreamOut() returned %lu!\r\n", (DWORD)mmrc);

                --pSeq->uBuffersInMMSYSTEM;
                pSeq->uState = SEQ_S_STOPPING;
            }
        }
    }
}

/***************************************************************************
*
* XlatSMFErr
*
* Translates an error from the SMF layer into an appropriate MCI error.
*
* smfrc                     - The return code from any SMF function.
*
* Returns
*   A parallel error from the MCI error codes.
*
***************************************************************************/
MMRESULT XlatSMFErr(SMFRESULT smfrc)
{
    switch (smfrc)
    {
        case SMF_SUCCESS:
            return MMSYSERR_NOERROR;

        case SMF_NO_MEMORY:
            return MCIERR_OUT_OF_MEMORY;

        case SMF_INVALID_FILE:
        case SMF_OPEN_FAILED:
        case SMF_INVALID_TRACK:
            return MCIERR_INVALID_FILE;

        default:
            return MCIERR_UNSUPPORTED_FUNCTION;
    }
}

/*****************************************************************************
*
* AllocBuffers
*
* Allocate and initialize the global sequencer structure.
*
*****************************************************************************/
BOOL AllocBuffers(HWND hWnd)
{
	if ((gpSeq = (PSEQ)LocalAlloc(LPTR, sizeof(SEQ))) == NULL)
		return FALSE;

	gpSeq->cBuffer  = C_MIDI_BUFFERS;
	gpSeq->cbBuffer = CB_MIDI_BUFFERS;

	if (seqAllocBuffers(gpSeq) != MMSYSERR_NOERROR)
		return FALSE;

	gpSeq->hWnd = hWnd;

	return TRUE;
}

/*****************************************************************************
*
* FreeBuffers
*
* Release any resources for the current instance.
*
*****************************************************************************/
void FreeBuffers(void)
{
	if (gpSeq != NULL)
	{
		seqFreeBuffers(gpSeq);
		LocalFree((HLOCAL)gpSeq);
		gpSeq = NULL;
	}
}

/*****************************************************************************
*
* AttemptFileOpen
*
* Try to open the given file in the sequencer.
*
* HWND hWnd                 - Application main window
*
* Stop and close the current file.
* Open the new file.
* Preroll the new file.
*
*****************************************************************************/
void AttemptFileOpen(HWND hWnd, LPSTR pstrName)
{
	MMRESULT mmrc;

	/* Stop, close, etc. if we're still playing
	*/
	printf("Calling seqStop(); state = %u\r\n", gpSeq->uState);
	mmrc = seqStop(gpSeq);
	if (mmrc != MMSYSERR_NOERROR)
	{
		//MessageBox(hWnd, "Attempt to stop playback failed", NULL, MB_ICONEXCLAMATION|MB_OK);
		return;
	}

	printf("Calling seqCloseFile(); state = %u\r\n", gpSeq->uState);
	seqCloseFile(gpSeq);

	printf("Calling seqOpenFile(); state = %u\r\n", gpSeq->uState);
	/* Open new file
	*/

	gpSeq->pstrFile = pstrName;
	mmrc = seqOpenFile(gpSeq);
	if (mmrc != MMSYSERR_NOERROR)
	{
		//MessageBox(hWnd, "Open of MIDI file failed", NULL, MB_ICONEXCLAMATION|MB_OK);
		return;
	}
}

/*****************************************************************************
*
* PrerollAndWait
*
* Prerolls the sequencer using the current device ID and file.
*
* HWND hWnd                 - Current window
*
* Just call preroll and loop processing messages until done.
*
*****************************************************************************/
BOOL PrerollAndWait(HWND hWnd)
{
	PREROLL preroll;
	MMRESULT mmrc;

	preroll.tkBase = 0;
	preroll.tkEnd  = gpSeq->tkLength;

	gpSeq->uDeviceID = 0;

	if (MMSYSERR_NOERROR != (mmrc = seqPreroll(gpSeq, &preroll)))
	{
		//MessageBox(hWnd, "Preroll of MIDI file failed", NULL, MB_ICONEXCLAMATION|MB_OK);
		return FALSE;
	}

	return TRUE;
}

/*****************************************************************************
*
* Start
*
* Starts playback at the current position.
*
*****************************************************************************/
void Start(void)
{
	seqStart(gpSeq);
}

/*****************************************************************************
*
* Stop
*
* Totally stops playback of an instance.
*
*****************************************************************************/
void Stop(void)
{
	if (gpSeq != NULL)
	{
		seqStop(gpSeq);
		gpSeq = NULL;
	}
}

#else

BOOL AllocBuffers(HWND hWnd)
{
	return FALSE;
}

void FreeBuffers(void)
{
}

void AttemptFileOpen(HWND hWnd, LPSTR pstrName)
{
}

BOOL PrerollAndWait(HWND hWnd)
{
	return FALSE;
}

void Start(void)
{
}

void Stop(void)
{
}

#endif // #if 0
