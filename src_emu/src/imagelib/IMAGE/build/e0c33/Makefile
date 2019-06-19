# make file made by wb33
USER_LIB_PATH = ..\lib
ROOT_PATH = d:\emulate\gui\image\
# macro definitions for tools & dir

TOOL_DIR = D:\compiler\e0c33
GCC33 = $(TOOL_DIR)\gcc33
PP33  = $(TOOL_DIR)\pp33
EXT33 = $(TOOL_DIR)\ext33
AS33  = $(TOOL_DIR)\as33
LK33  = $(TOOL_DIR)\lk33
LIB33  = $(TOOL_DIR)\lib33
MAKE  = $(TOOL_DIR)\make
SRC_DIR = 

# macro definitions for tool flags

GCC33_FLAG = -B$(TOOL_DIR)\ -S -g -O -fno-builtin -I../../../include -I../../../sysinc
PP33_FLAG  = -g 
EXT33_FLAG = 
AS33_FLAG  = -g 
LK33_FLAG  = -g -s -m -c 
EXT33_CMX_FLAG = -lk test -c 
LIB33  = $(TOOL_DIR)\lib33
MAKE  = $(TOOL_DIR)\make
LIB33_FLAG = -a

# suffix & rule definitions

.SUFFIXES : .c .s .ps .ms .o .srf

.c.ms :
	$(GCC33) $(GCC33_FLAG) $(SRC_DIR)$*.c
	$(EXT33) $(EXT33_FLAG) $*.ps

.s.ms :
	$(PP33)  $(PP33_FLAG)  $(SRC_DIR)$*.s
	$(EXT33) $(EXT33_FLAG) $*.ps

.ms.o :
	$(AS33)  $(AS33_FLAG)  $*.ms

# dependency list start

### src definition start
SRC1_DIR=$(ROOT_PATH)\src\\

SRC2_DIR=$(ROOT_PATH)\src\Gif\\

SRC3_DIR=$(ROOT_PATH)\src\Jpeg\\

SRC4_DIR=$(ROOT_PATH)\src\png\\

### src definition end

PLXimage.lib : BmpCvert.o \
    DrawImg.o \
    DrawJpeg.o \
    DrawPng.o \
    drawwbmp.o \
    filesys.o \
    PLXimg.o \
    dgif.o \
    EGIF_LIB.o \
    gif_fsm.o \
    GIF_HASH.o \
    gif_lwz.o \
    mymemmgr.o \
    djpeg.o \
    JCAPIMIN.o \
    JCAPISTD.o \
    JCCOEFCT.o \
    JCCOLOR.o \
    JCDCTMGR.o \
    JCHUFF.o \
    JCINIT.o \
    JCMAINCT.o \
    JCMARKER.o \
    JCMASTER.o \
    jcomapi.o \
    JCPARAM.o \
    JCPHUFF.o \
    JCPREPCT.o \
    JCSAMPLE.o \
    JCTRANS.o \
    jdapimin.o \
    jdapistd.o \
    JDATADST.o \
    JDATASRC.o \
    jdcoefct.o \
    jdcolor.o \
    jddctmgr.o \
    jdhuff.o \
    jdinput.o \
    jdmainct.o \
    jdmarker.o \
    jdmaster.o \
    JDMERGE.o \
    jdphuff.o \
    jdpostct.o \
    jdsample.o \
    JDTRANS.o \
    JERROR.o \
    JFDCTFLT.o \
    JFDCTFST.o \
    JFDCTINT.o \
    JIDCTFLT.o \
    jidctfst.o \
    jidctint.o \
    JIDCTRED.o \
    jmemmgr.o \
    JMEMNOBS.o \
    JPEGTRAN.o \
    JQUANT1.o \
    JQUANT2.o \
    jutils.o \
    png.o \
    pngerror.o \
    pngget.o \
    pngmem.o \
    pngpread.o \
    pngread.o \
    pngrio.o \
    pngrtran.o \
    pngrutil.o \
    pngset.o \
    pngtrans.o \
    pngvcrd.o \
    pngwio.o \
    pngwrite.o \
    pngwtran.o \
    pngwutil.o \
 
	$(LIB33) $(LIB33_FLAG) PLXimage.lib BmpCvert.o \
    DrawImg.o \
    DrawJpeg.o \
    DrawPng.o \
    drawwbmp.o \
    filesys.o \
    PLXimg.o \
    dgif.o \
    EGIF_LIB.o \
    gif_fsm.o \
    GIF_HASH.o \
    gif_lwz.o \
    mymemmgr.o \
    djpeg.o 
	$(LIB33) $(LIB33_FLAG) PLXimage.lib JCAPIMIN.o \
    JCAPISTD.o \
    JCCOEFCT.o \
    JCCOLOR.o \
    JCDCTMGR.o \
    JCHUFF.o \
    JCINIT.o \
    JCMAINCT.o \
    JCMARKER.o \
    JCMASTER.o \
    jcomapi.o \
    JCPARAM.o \
    JCPHUFF.o \
    JCPREPCT.o \
    JCSAMPLE.o \
    JCTRANS.o \
    jdapimin.o \
    jdapistd.o \
    JDATADST.o \
    JDATASRC.o \
    jdcoefct.o \
    jdcolor.o \
    jddctmgr.o \
    jdhuff.o \
    jdinput.o \
    jdmainct.o \
    jdmarker.o \
    jdmaster.o \
    JDMERGE.o \
    jdphuff.o \
    jdpostct.o \
    jdsample.o \
    JDTRANS.o \
    JERROR.o \
    JFDCTFLT.o \
    JFDCTFST.o \
    JFDCTINT.o \
    JIDCTFLT.o \
    jidctfst.o \
    jidctint.o \
    JIDCTRED.o \
    jmemmgr.o \
    JMEMNOBS.o \
    JPEGTRAN.o \
    JQUANT1.o \
    JQUANT2.o \
    jutils.o \
    png.o \
    pngerror.o \
    pngget.o \
    pngmem.o \
    pngpread.o \
    pngread.o \
    pngrio.o \
    pngrtran.o \
    pngrutil.o \
    pngset.o \
    pngtrans.o \
    pngvcrd.o \
    pngwio.o \
    pngwrite.o \
    pngwtran.o \
    pngwutil.o \

	copy PLXimage.lib $(USER_LIB_PATH)
	del *.lib

## BmpCvert.c
BmpCvert.ms : $(SRC1_DIR)BmpCvert.c
	$(GCC33) $(GCC33_FLAG) $(SRC1_DIR)BmpCvert.c
	$(EXT33) $(EXT33_FLAG) BmpCvert.ps
BmpCvert.o : BmpCvert.ms
	$(AS33) $(AS33_FLAG) BmpCvert.ms

## DrawImg.c
DrawImg.ms : $(SRC1_DIR)DrawImg.c
	$(GCC33) $(GCC33_FLAG) $(SRC1_DIR)DrawImg.c
	$(EXT33) $(EXT33_FLAG) DrawImg.ps
DrawImg.o : DrawImg.ms
	$(AS33) $(AS33_FLAG) DrawImg.ms

## DrawJpeg.c
DrawJpeg.ms : $(SRC1_DIR)DrawJpeg.c
	$(GCC33) $(GCC33_FLAG) $(SRC1_DIR)DrawJpeg.c
	$(EXT33) $(EXT33_FLAG) DrawJpeg.ps
DrawJpeg.o : DrawJpeg.ms
	$(AS33) $(AS33_FLAG) DrawJpeg.ms

## DrawPng.c
DrawPng.ms : $(SRC1_DIR)DrawPng.c
	$(GCC33) $(GCC33_FLAG) $(SRC1_DIR)DrawPng.c
	$(EXT33) $(EXT33_FLAG) DrawPng.ps
DrawPng.o : DrawPng.ms
	$(AS33) $(AS33_FLAG) DrawPng.ms

## drawwbmp.c
drawwbmp.ms : $(SRC1_DIR)drawwbmp.c
	$(GCC33) $(GCC33_FLAG) $(SRC1_DIR)drawwbmp.c
	$(EXT33) $(EXT33_FLAG) drawwbmp.ps
drawwbmp.o : drawwbmp.ms
	$(AS33) $(AS33_FLAG) drawwbmp.ms

## filesys.c
filesys.ms : $(SRC1_DIR)filesys.c
	$(GCC33) $(GCC33_FLAG) $(SRC1_DIR)filesys.c
	$(EXT33) $(EXT33_FLAG) filesys.ps
filesys.o : filesys.ms
	$(AS33) $(AS33_FLAG) filesys.ms

## PLXimg.c
PLXimg.ms : $(SRC1_DIR)PLXimg.c
	$(GCC33) $(GCC33_FLAG) $(SRC1_DIR)PLXimg.c
	$(EXT33) $(EXT33_FLAG) PLXimg.ps
PLXimg.o : PLXimg.ms
	$(AS33) $(AS33_FLAG) PLXimg.ms

## dgif.c
dgif.ms : $(SRC2_DIR)dgif.c
	$(GCC33) $(GCC33_FLAG) $(SRC2_DIR)dgif.c
	$(EXT33) $(EXT33_FLAG) dgif.ps
dgif.o : dgif.ms
	$(AS33) $(AS33_FLAG) dgif.ms

## EGIF_LIB.c
EGIF_LIB.ms : $(SRC2_DIR)EGIF_LIB.c
	$(GCC33) $(GCC33_FLAG) $(SRC2_DIR)EGIF_LIB.c
	$(EXT33) $(EXT33_FLAG) EGIF_LIB.ps
EGIF_LIB.o : EGIF_LIB.ms
	$(AS33) $(AS33_FLAG) EGIF_LIB.ms

## gif_fsm.c
gif_fsm.ms : $(SRC2_DIR)gif_fsm.c
	$(GCC33) $(GCC33_FLAG) $(SRC2_DIR)gif_fsm.c
	$(EXT33) $(EXT33_FLAG) gif_fsm.ps
gif_fsm.o : gif_fsm.ms
	$(AS33) $(AS33_FLAG) gif_fsm.ms

## GIF_HASH.c
GIF_HASH.ms : $(SRC2_DIR)GIF_HASH.c
	$(GCC33) $(GCC33_FLAG) $(SRC2_DIR)GIF_HASH.c
	$(EXT33) $(EXT33_FLAG) GIF_HASH.ps
GIF_HASH.o : GIF_HASH.ms
	$(AS33) $(AS33_FLAG) GIF_HASH.ms

## gif_lwz.c
gif_lwz.ms : $(SRC2_DIR)gif_lwz.c
	$(GCC33) $(GCC33_FLAG) $(SRC2_DIR)gif_lwz.c
	$(EXT33) $(EXT33_FLAG) gif_lwz.ps
gif_lwz.o : gif_lwz.ms
	$(AS33) $(AS33_FLAG) gif_lwz.ms

## mymemmgr.c
mymemmgr.ms : $(SRC2_DIR)mymemmgr.c
	$(GCC33) $(GCC33_FLAG) $(SRC2_DIR)mymemmgr.c
	$(EXT33) $(EXT33_FLAG) mymemmgr.ps
mymemmgr.o : mymemmgr.ms
	$(AS33) $(AS33_FLAG) mymemmgr.ms

## djpeg.c
djpeg.ms : $(SRC3_DIR)djpeg.c
	$(GCC33) $(GCC33_FLAG) $(SRC3_DIR)djpeg.c
	$(EXT33) $(EXT33_FLAG) djpeg.ps
djpeg.o : djpeg.ms
	$(AS33) $(AS33_FLAG) djpeg.ms

## JCAPIMIN.c
JCAPIMIN.ms : $(SRC3_DIR)JCAPIMIN.c
	$(GCC33) $(GCC33_FLAG) $(SRC3_DIR)JCAPIMIN.c
	$(EXT33) $(EXT33_FLAG) JCAPIMIN.ps
JCAPIMIN.o : JCAPIMIN.ms
	$(AS33) $(AS33_FLAG) JCAPIMIN.ms

## JCAPISTD.c
JCAPISTD.ms : $(SRC3_DIR)JCAPISTD.c
	$(GCC33) $(GCC33_FLAG) $(SRC3_DIR)JCAPISTD.c
	$(EXT33) $(EXT33_FLAG) JCAPISTD.ps
JCAPISTD.o : JCAPISTD.ms
	$(AS33) $(AS33_FLAG) JCAPISTD.ms

## JCCOEFCT.c
JCCOEFCT.ms : $(SRC3_DIR)JCCOEFCT.c
	$(GCC33) $(GCC33_FLAG) $(SRC3_DIR)JCCOEFCT.c
	$(EXT33) $(EXT33_FLAG) JCCOEFCT.ps
JCCOEFCT.o : JCCOEFCT.ms
	$(AS33) $(AS33_FLAG) JCCOEFCT.ms

## JCCOLOR.c
JCCOLOR.ms : $(SRC3_DIR)JCCOLOR.c
	$(GCC33) $(GCC33_FLAG) $(SRC3_DIR)JCCOLOR.c
	$(EXT33) $(EXT33_FLAG) JCCOLOR.ps
JCCOLOR.o : JCCOLOR.ms
	$(AS33) $(AS33_FLAG) JCCOLOR.ms

## JCDCTMGR.c
JCDCTMGR.ms : $(SRC3_DIR)JCDCTMGR.c
	$(GCC33) $(GCC33_FLAG) $(SRC3_DIR)JCDCTMGR.c
	$(EXT33) $(EXT33_FLAG) JCDCTMGR.ps
JCDCTMGR.o : JCDCTMGR.ms
	$(AS33) $(AS33_FLAG) JCDCTMGR.ms

## JCHUFF.c
JCHUFF.ms : $(SRC3_DIR)JCHUFF.c
	$(GCC33) $(GCC33_FLAG) $(SRC3_DIR)JCHUFF.c
	$(EXT33) $(EXT33_FLAG) JCHUFF.ps
JCHUFF.o : JCHUFF.ms
	$(AS33) $(AS33_FLAG) JCHUFF.ms

## JCINIT.c
JCINIT.ms : $(SRC3_DIR)JCINIT.c
	$(GCC33) $(GCC33_FLAG) $(SRC3_DIR)JCINIT.c
	$(EXT33) $(EXT33_FLAG) JCINIT.ps
JCINIT.o : JCINIT.ms
	$(AS33) $(AS33_FLAG) JCINIT.ms

## JCMAINCT.c
JCMAINCT.ms : $(SRC3_DIR)JCMAINCT.c
	$(GCC33) $(GCC33_FLAG) $(SRC3_DIR)JCMAINCT.c
	$(EXT33) $(EXT33_FLAG) JCMAINCT.ps
JCMAINCT.o : JCMAINCT.ms
	$(AS33) $(AS33_FLAG) JCMAINCT.ms

## JCMARKER.c
JCMARKER.ms : $(SRC3_DIR)JCMARKER.c
	$(GCC33) $(GCC33_FLAG) $(SRC3_DIR)JCMARKER.c
	$(EXT33) $(EXT33_FLAG) JCMARKER.ps
JCMARKER.o : JCMARKER.ms
	$(AS33) $(AS33_FLAG) JCMARKER.ms

## JCMASTER.c
JCMASTER.ms : $(SRC3_DIR)JCMASTER.c
	$(GCC33) $(GCC33_FLAG) $(SRC3_DIR)JCMASTER.c
	$(EXT33) $(EXT33_FLAG) JCMASTER.ps
JCMASTER.o : JCMASTER.ms
	$(AS33) $(AS33_FLAG) JCMASTER.ms

## jcomapi.c
jcomapi.ms : $(SRC3_DIR)jcomapi.c
	$(GCC33) $(GCC33_FLAG) $(SRC3_DIR)jcomapi.c
	$(EXT33) $(EXT33_FLAG) jcomapi.ps
jcomapi.o : jcomapi.ms
	$(AS33) $(AS33_FLAG) jcomapi.ms

## JCPARAM.c
JCPARAM.ms : $(SRC3_DIR)JCPARAM.c
	$(GCC33) $(GCC33_FLAG) $(SRC3_DIR)JCPARAM.c
	$(EXT33) $(EXT33_FLAG) JCPARAM.ps
JCPARAM.o : JCPARAM.ms
	$(AS33) $(AS33_FLAG) JCPARAM.ms

## JCPHUFF.c
JCPHUFF.ms : $(SRC3_DIR)JCPHUFF.c
	$(GCC33) $(GCC33_FLAG) $(SRC3_DIR)JCPHUFF.c
	$(EXT33) $(EXT33_FLAG) JCPHUFF.ps
JCPHUFF.o : JCPHUFF.ms
	$(AS33) $(AS33_FLAG) JCPHUFF.ms

## JCPREPCT.c
JCPREPCT.ms : $(SRC3_DIR)JCPREPCT.c
	$(GCC33) $(GCC33_FLAG) $(SRC3_DIR)JCPREPCT.c
	$(EXT33) $(EXT33_FLAG) JCPREPCT.ps
JCPREPCT.o : JCPREPCT.ms
	$(AS33) $(AS33_FLAG) JCPREPCT.ms

## JCSAMPLE.c
JCSAMPLE.ms : $(SRC3_DIR)JCSAMPLE.c
	$(GCC33) $(GCC33_FLAG) $(SRC3_DIR)JCSAMPLE.c
	$(EXT33) $(EXT33_FLAG) JCSAMPLE.ps
JCSAMPLE.o : JCSAMPLE.ms
	$(AS33) $(AS33_FLAG) JCSAMPLE.ms

## JCTRANS.c
JCTRANS.ms : $(SRC3_DIR)JCTRANS.c
	$(GCC33) $(GCC33_FLAG) $(SRC3_DIR)JCTRANS.c
	$(EXT33) $(EXT33_FLAG) JCTRANS.ps
JCTRANS.o : JCTRANS.ms
	$(AS33) $(AS33_FLAG) JCTRANS.ms

## jdapimin.c
jdapimin.ms : $(SRC3_DIR)jdapimin.c
	$(GCC33) $(GCC33_FLAG) $(SRC3_DIR)jdapimin.c
	$(EXT33) $(EXT33_FLAG) jdapimin.ps
jdapimin.o : jdapimin.ms
	$(AS33) $(AS33_FLAG) jdapimin.ms

## jdapistd.c
jdapistd.ms : $(SRC3_DIR)jdapistd.c
	$(GCC33) $(GCC33_FLAG) $(SRC3_DIR)jdapistd.c
	$(EXT33) $(EXT33_FLAG) jdapistd.ps
jdapistd.o : jdapistd.ms
	$(AS33) $(AS33_FLAG) jdapistd.ms

## JDATADST.c
JDATADST.ms : $(SRC3_DIR)JDATADST.c
	$(GCC33) $(GCC33_FLAG) $(SRC3_DIR)JDATADST.c
	$(EXT33) $(EXT33_FLAG) JDATADST.ps
JDATADST.o : JDATADST.ms
	$(AS33) $(AS33_FLAG) JDATADST.ms

## JDATASRC.c
JDATASRC.ms : $(SRC3_DIR)JDATASRC.c
	$(GCC33) $(GCC33_FLAG) $(SRC3_DIR)JDATASRC.c
	$(EXT33) $(EXT33_FLAG) JDATASRC.ps
JDATASRC.o : JDATASRC.ms
	$(AS33) $(AS33_FLAG) JDATASRC.ms

## jdcoefct.c
jdcoefct.ms : $(SRC3_DIR)jdcoefct.c
	$(GCC33) $(GCC33_FLAG) $(SRC3_DIR)jdcoefct.c
	$(EXT33) $(EXT33_FLAG) jdcoefct.ps
jdcoefct.o : jdcoefct.ms
	$(AS33) $(AS33_FLAG) jdcoefct.ms

## jdcolor.c
jdcolor.ms : $(SRC3_DIR)jdcolor.c
	$(GCC33) $(GCC33_FLAG) $(SRC3_DIR)jdcolor.c
	$(EXT33) $(EXT33_FLAG) jdcolor.ps
jdcolor.o : jdcolor.ms
	$(AS33) $(AS33_FLAG) jdcolor.ms

## jddctmgr.c
jddctmgr.ms : $(SRC3_DIR)jddctmgr.c
	$(GCC33) $(GCC33_FLAG) $(SRC3_DIR)jddctmgr.c
	$(EXT33) $(EXT33_FLAG) jddctmgr.ps
jddctmgr.o : jddctmgr.ms
	$(AS33) $(AS33_FLAG) jddctmgr.ms

## jdhuff.c
jdhuff.ms : $(SRC3_DIR)jdhuff.c
	$(GCC33) $(GCC33_FLAG) $(SRC3_DIR)jdhuff.c
	$(EXT33) $(EXT33_FLAG) jdhuff.ps
jdhuff.o : jdhuff.ms
	$(AS33) $(AS33_FLAG) jdhuff.ms

## jdinput.c
jdinput.ms : $(SRC3_DIR)jdinput.c
	$(GCC33) $(GCC33_FLAG) $(SRC3_DIR)jdinput.c
	$(EXT33) $(EXT33_FLAG) jdinput.ps
jdinput.o : jdinput.ms
	$(AS33) $(AS33_FLAG) jdinput.ms

## jdmainct.c
jdmainct.ms : $(SRC3_DIR)jdmainct.c
	$(GCC33) $(GCC33_FLAG) $(SRC3_DIR)jdmainct.c
	$(EXT33) $(EXT33_FLAG) jdmainct.ps
jdmainct.o : jdmainct.ms
	$(AS33) $(AS33_FLAG) jdmainct.ms

## jdmarker.c
jdmarker.ms : $(SRC3_DIR)jdmarker.c
	$(GCC33) $(GCC33_FLAG) $(SRC3_DIR)jdmarker.c
	$(EXT33) $(EXT33_FLAG) jdmarker.ps
jdmarker.o : jdmarker.ms
	$(AS33) $(AS33_FLAG) jdmarker.ms

## jdmaster.c
jdmaster.ms : $(SRC3_DIR)jdmaster.c
	$(GCC33) $(GCC33_FLAG) $(SRC3_DIR)jdmaster.c
	$(EXT33) $(EXT33_FLAG) jdmaster.ps
jdmaster.o : jdmaster.ms
	$(AS33) $(AS33_FLAG) jdmaster.ms

## JDMERGE.c
JDMERGE.ms : $(SRC3_DIR)JDMERGE.c
	$(GCC33) $(GCC33_FLAG) $(SRC3_DIR)JDMERGE.c
	$(EXT33) $(EXT33_FLAG) JDMERGE.ps
JDMERGE.o : JDMERGE.ms
	$(AS33) $(AS33_FLAG) JDMERGE.ms

## jdphuff.c
jdphuff.ms : $(SRC3_DIR)jdphuff.c
	$(GCC33) $(GCC33_FLAG) $(SRC3_DIR)jdphuff.c
	$(EXT33) $(EXT33_FLAG) jdphuff.ps
jdphuff.o : jdphuff.ms
	$(AS33) $(AS33_FLAG) jdphuff.ms

## jdpostct.c
jdpostct.ms : $(SRC3_DIR)jdpostct.c
	$(GCC33) $(GCC33_FLAG) $(SRC3_DIR)jdpostct.c
	$(EXT33) $(EXT33_FLAG) jdpostct.ps
jdpostct.o : jdpostct.ms
	$(AS33) $(AS33_FLAG) jdpostct.ms

## jdsample.c
jdsample.ms : $(SRC3_DIR)jdsample.c
	$(GCC33) $(GCC33_FLAG) $(SRC3_DIR)jdsample.c
	$(EXT33) $(EXT33_FLAG) jdsample.ps
jdsample.o : jdsample.ms
	$(AS33) $(AS33_FLAG) jdsample.ms

## JDTRANS.c
JDTRANS.ms : $(SRC3_DIR)JDTRANS.c
	$(GCC33) $(GCC33_FLAG) $(SRC3_DIR)JDTRANS.c
	$(EXT33) $(EXT33_FLAG) JDTRANS.ps
JDTRANS.o : JDTRANS.ms
	$(AS33) $(AS33_FLAG) JDTRANS.ms

## JERROR.c
JERROR.ms : $(SRC3_DIR)JERROR.c
	$(GCC33) $(GCC33_FLAG) $(SRC3_DIR)JERROR.c
	$(EXT33) $(EXT33_FLAG) JERROR.ps
JERROR.o : JERROR.ms
	$(AS33) $(AS33_FLAG) JERROR.ms

## JFDCTFLT.c
JFDCTFLT.ms : $(SRC3_DIR)JFDCTFLT.c
	$(GCC33) $(GCC33_FLAG) $(SRC3_DIR)JFDCTFLT.c
	$(EXT33) $(EXT33_FLAG) JFDCTFLT.ps
JFDCTFLT.o : JFDCTFLT.ms
	$(AS33) $(AS33_FLAG) JFDCTFLT.ms

## JFDCTFST.c
JFDCTFST.ms : $(SRC3_DIR)JFDCTFST.c
	$(GCC33) $(GCC33_FLAG) $(SRC3_DIR)JFDCTFST.c
	$(EXT33) $(EXT33_FLAG) JFDCTFST.ps
JFDCTFST.o : JFDCTFST.ms
	$(AS33) $(AS33_FLAG) JFDCTFST.ms

## JFDCTINT.c
JFDCTINT.ms : $(SRC3_DIR)JFDCTINT.c
	$(GCC33) $(GCC33_FLAG) $(SRC3_DIR)JFDCTINT.c
	$(EXT33) $(EXT33_FLAG) JFDCTINT.ps
JFDCTINT.o : JFDCTINT.ms
	$(AS33) $(AS33_FLAG) JFDCTINT.ms

## JIDCTFLT.c
JIDCTFLT.ms : $(SRC3_DIR)JIDCTFLT.c
	$(GCC33) $(GCC33_FLAG) $(SRC3_DIR)JIDCTFLT.c
	$(EXT33) $(EXT33_FLAG) JIDCTFLT.ps
JIDCTFLT.o : JIDCTFLT.ms
	$(AS33) $(AS33_FLAG) JIDCTFLT.ms

## jidctfst.c
jidctfst.ms : $(SRC3_DIR)jidctfst.c
	$(GCC33) $(GCC33_FLAG) $(SRC3_DIR)jidctfst.c
	$(EXT33) $(EXT33_FLAG) jidctfst.ps
jidctfst.o : jidctfst.ms
	$(AS33) $(AS33_FLAG) jidctfst.ms

## jidctint.c
jidctint.ms : $(SRC3_DIR)jidctint.c
	$(GCC33) $(GCC33_FLAG) $(SRC3_DIR)jidctint.c
	$(EXT33) $(EXT33_FLAG) jidctint.ps
jidctint.o : jidctint.ms
	$(AS33) $(AS33_FLAG) jidctint.ms

## JIDCTRED.c
JIDCTRED.ms : $(SRC3_DIR)JIDCTRED.c
	$(GCC33) $(GCC33_FLAG) $(SRC3_DIR)JIDCTRED.c
	$(EXT33) $(EXT33_FLAG) JIDCTRED.ps
JIDCTRED.o : JIDCTRED.ms
	$(AS33) $(AS33_FLAG) JIDCTRED.ms

## jmemmgr.c
jmemmgr.ms : $(SRC3_DIR)jmemmgr.c
	$(GCC33) $(GCC33_FLAG) $(SRC3_DIR)jmemmgr.c
	$(EXT33) $(EXT33_FLAG) jmemmgr.ps
jmemmgr.o : jmemmgr.ms
	$(AS33) $(AS33_FLAG) jmemmgr.ms

## JMEMNOBS.c
JMEMNOBS.ms : $(SRC3_DIR)JMEMNOBS.c
	$(GCC33) $(GCC33_FLAG) $(SRC3_DIR)JMEMNOBS.c
	$(EXT33) $(EXT33_FLAG) JMEMNOBS.ps
JMEMNOBS.o : JMEMNOBS.ms
	$(AS33) $(AS33_FLAG) JMEMNOBS.ms

## JPEGTRAN.c
JPEGTRAN.ms : $(SRC3_DIR)JPEGTRAN.c
	$(GCC33) $(GCC33_FLAG) $(SRC3_DIR)JPEGTRAN.c
	$(EXT33) $(EXT33_FLAG) JPEGTRAN.ps
JPEGTRAN.o : JPEGTRAN.ms
	$(AS33) $(AS33_FLAG) JPEGTRAN.ms

## JQUANT1.c
JQUANT1.ms : $(SRC3_DIR)JQUANT1.c
	$(GCC33) $(GCC33_FLAG) $(SRC3_DIR)JQUANT1.c
	$(EXT33) $(EXT33_FLAG) JQUANT1.ps
JQUANT1.o : JQUANT1.ms
	$(AS33) $(AS33_FLAG) JQUANT1.ms

## JQUANT2.c
JQUANT2.ms : $(SRC3_DIR)JQUANT2.c
	$(GCC33) $(GCC33_FLAG) $(SRC3_DIR)JQUANT2.c
	$(EXT33) $(EXT33_FLAG) JQUANT2.ps
JQUANT2.o : JQUANT2.ms
	$(AS33) $(AS33_FLAG) JQUANT2.ms

## jutils.c
jutils.ms : $(SRC3_DIR)jutils.c
	$(GCC33) $(GCC33_FLAG) $(SRC3_DIR)jutils.c
	$(EXT33) $(EXT33_FLAG) jutils.ps
jutils.o : jutils.ms
	$(AS33) $(AS33_FLAG) jutils.ms

## png.c
png.ms : $(SRC4_DIR)png.c
	$(GCC33) $(GCC33_FLAG) $(SRC4_DIR)png.c
	$(EXT33) $(EXT33_FLAG) png.ps
png.o : png.ms
	$(AS33) $(AS33_FLAG) png.ms

## pngerror.c
pngerror.ms : $(SRC4_DIR)pngerror.c
	$(GCC33) $(GCC33_FLAG) $(SRC4_DIR)pngerror.c
	$(EXT33) $(EXT33_FLAG) pngerror.ps
pngerror.o : pngerror.ms
	$(AS33) $(AS33_FLAG) pngerror.ms

## pngget.c
pngget.ms : $(SRC4_DIR)pngget.c
	$(GCC33) $(GCC33_FLAG) $(SRC4_DIR)pngget.c
	$(EXT33) $(EXT33_FLAG) pngget.ps
pngget.o : pngget.ms
	$(AS33) $(AS33_FLAG) pngget.ms

## pngmem.c
pngmem.ms : $(SRC4_DIR)pngmem.c
	$(GCC33) $(GCC33_FLAG) $(SRC4_DIR)pngmem.c
	$(EXT33) $(EXT33_FLAG) pngmem.ps
pngmem.o : pngmem.ms
	$(AS33) $(AS33_FLAG) pngmem.ms

## pngpread.c
pngpread.ms : $(SRC4_DIR)pngpread.c
	$(GCC33) $(GCC33_FLAG) $(SRC4_DIR)pngpread.c
	$(EXT33) $(EXT33_FLAG) pngpread.ps
pngpread.o : pngpread.ms
	$(AS33) $(AS33_FLAG) pngpread.ms

## pngread.c
pngread.ms : $(SRC4_DIR)pngread.c
	$(GCC33) $(GCC33_FLAG) $(SRC4_DIR)pngread.c
	$(EXT33) $(EXT33_FLAG) pngread.ps
pngread.o : pngread.ms
	$(AS33) $(AS33_FLAG) pngread.ms

## pngrio.c
pngrio.ms : $(SRC4_DIR)pngrio.c
	$(GCC33) $(GCC33_FLAG) $(SRC4_DIR)pngrio.c
	$(EXT33) $(EXT33_FLAG) pngrio.ps
pngrio.o : pngrio.ms
	$(AS33) $(AS33_FLAG) pngrio.ms

## pngrtran.c
pngrtran.ms : $(SRC4_DIR)pngrtran.c
	$(GCC33) $(GCC33_FLAG) $(SRC4_DIR)pngrtran.c
	$(EXT33) $(EXT33_FLAG) pngrtran.ps
pngrtran.o : pngrtran.ms
	$(AS33) $(AS33_FLAG) pngrtran.ms

## pngrutil.c
pngrutil.ms : $(SRC4_DIR)pngrutil.c
	$(GCC33) $(GCC33_FLAG) $(SRC4_DIR)pngrutil.c
	$(EXT33) $(EXT33_FLAG) pngrutil.ps
pngrutil.o : pngrutil.ms
	$(AS33) $(AS33_FLAG) pngrutil.ms

## pngset.c
pngset.ms : $(SRC4_DIR)pngset.c
	$(GCC33) $(GCC33_FLAG) $(SRC4_DIR)pngset.c
	$(EXT33) $(EXT33_FLAG) pngset.ps
pngset.o : pngset.ms
	$(AS33) $(AS33_FLAG) pngset.ms

## pngtrans.c
pngtrans.ms : $(SRC4_DIR)pngtrans.c
	$(GCC33) $(GCC33_FLAG) $(SRC4_DIR)pngtrans.c
	$(EXT33) $(EXT33_FLAG) pngtrans.ps
pngtrans.o : pngtrans.ms
	$(AS33) $(AS33_FLAG) pngtrans.ms

## pngvcrd.c
pngvcrd.ms : $(SRC4_DIR)pngvcrd.c
	$(GCC33) $(GCC33_FLAG) $(SRC4_DIR)pngvcrd.c
	$(EXT33) $(EXT33_FLAG) pngvcrd.ps
pngvcrd.o : pngvcrd.ms
	$(AS33) $(AS33_FLAG) pngvcrd.ms

## pngwio.c
pngwio.ms : $(SRC4_DIR)pngwio.c
	$(GCC33) $(GCC33_FLAG) $(SRC4_DIR)pngwio.c
	$(EXT33) $(EXT33_FLAG) pngwio.ps
pngwio.o : pngwio.ms
	$(AS33) $(AS33_FLAG) pngwio.ms

## pngwrite.c
pngwrite.ms : $(SRC4_DIR)pngwrite.c
	$(GCC33) $(GCC33_FLAG) $(SRC4_DIR)pngwrite.c
	$(EXT33) $(EXT33_FLAG) pngwrite.ps
pngwrite.o : pngwrite.ms
	$(AS33) $(AS33_FLAG) pngwrite.ms

## pngwtran.c
pngwtran.ms : $(SRC4_DIR)pngwtran.c
	$(GCC33) $(GCC33_FLAG) $(SRC4_DIR)pngwtran.c
	$(EXT33) $(EXT33_FLAG) pngwtran.ps
pngwtran.o : pngwtran.ms
	$(AS33) $(AS33_FLAG) pngwtran.ms

## pngwutil.c
pngwutil.ms : $(SRC4_DIR)pngwutil.c
	$(GCC33) $(GCC33_FLAG) $(SRC4_DIR)pngwutil.c
	$(EXT33) $(EXT33_FLAG) pngwutil.ps
pngwutil.o : pngwutil.ms
	$(AS33) $(AS33_FLAG) pngwutil.ms

# dependency list end

# optimaization by 2 pass make

opt:
	$(MAKE) -f test.mak
	$(TOOL_DIR)\cwait 2
	$(EXT33) $(EXT33_CMX_FLAG) test.cmx
	$(MAKE) -f test.mak

# clean files except source

clean:
	del *.o
	del *.ms
	del *.ps
