ifeq ($(comp),)
comp=xscale-elf
else
comp=m68k-elf
endif

CC = $(comp)-gcc 
AR = $(comp)-ar
RANLIB = $(comp)-ar -s
LD = $(comp)-ld
STRIP = $(comp)-strip

TARGET 		:= libimage.a

RM-F := rm -f

PATH_PRJ 	= ../../..
PATH_EMU 	= $(PATH_PRJ)
PATH_REAL 	= $(PATH_PRJ)

SDK_LIB 	= $(PATH_REAL)/hpimage/build/lib
INC30 		= $(PATH_EMU)/include
INC20 		= $(PATH_EMU)/include20
INCDDI 		= $(PATH_EMU)/include20/ddi

SRC_PATH 	= $(PATH_EMU)/hpimage/src
INCPATH30 	= $(PATH_PRJ) $(INC30)
INCPATH20 	= $(PATH_PRJ) $(INC20) $(INCDDI)

CFLAGS_DEBUG30 	= -g -Wall -nostdinc -march=armv4 -mtune=arm9tdmi $(addprefix -I, $(INCPATH30)) $(addprefix -D, $(MACRO)) -D __ST_HOPEN_GUI=1
CFLAGS_RELEASE30	= -O2 -Wall -march=armv4 -mtune=arm9tdmi -I$(addprefix -I, $(INCPATH30))
CFLAGS_DEBUG20 	= -g -Wall -nostdinc -march=armv4 -mtune=arm9tdmi $(addprefix -I, $(INCPATH20)) $(addprefix -D, $(MACRO)) -D __ST_HOPEN_GUI=1
CFLAGS_RELEASE20	= -O2 -Wall -march=armv4 -mtune=arm9tdmi -I$(addprefix -I, $(INCPATH20))

OBJ_ROOT	= hpimg.o DrawJpeg.o BmpCvert.o DrawPng.o drawwbmp.o DrawImg.o
OBJ_FILE	= filesys.o
OBJ_GIF		= dgif.o gif_fsm.o gif_lwz.o mymemmgr.o EGIF_LIB.o GIF_HASH.o
OBJ_JPEG	= djpeg.o JCAPIMIN.o JCAPISTD.o	JCCOEFCT.o JCCOLOR.o JCDCTMGR.o	JCHUFF.o\
		JCINIT.o JCMAINCT.o JCMARKER.o JCMASTER.o jcomapi.o JCPARAM.o JCPHUFF.o\
		JCPREPCT.o JCSAMPLE.o JCTRANS.o jdapimin.o jdapistd.o JDATADST.o JDATASRC.o\
		jdcoefct.o jdcolor.o jddctmgr.o jdhuff.o jdinput.o jdmainct.o jdmarker.o\
		jdmaster.o JDMERGE.o jdphuff.o jdpostct.o jdsample.o JDTRANS.o JERROR.o\
		JFDCTFLT.o JFDCTFST.o JIDCTFLT.o jidctfst.o jmemmgr.o JMEMNOBS.o JPEGTRAN.o\
		 JQUANT1.o JQUANT2.o jutils.o jidctint.o JIDCTRED.o JFDCTINT.o   
OBJ_PNG		= png.o pngerror.o pngget.o pngmem.o pngpread.o pngread.o pngrio.o pngrtran.o\
		pngrutil.o pngset.o pngtrans.o pngvcrd.o pngwio.o pngwrite.o pngwtran.o pngwutil.o	

OBJS		= $(OBJ_ROOT) $(OBJ_FILE) $(OBJ_GIF) $(OBJ_JPEG) $(OBJ_PNG)

DIR_ROOT	= $(SRC_PATH)
DIR_GIF		= $(SRC_PATH)/gif
DIR_JPEG	= $(SRC_PATH)/jpeg
DIR_PNG		= $(SRC_PATH)/png


all : $(TARGET)

$(OBJ_ROOT) : %.o : $(DIR_ROOT)/%.c
	$(CC) -c $(CFLAGS_DEBUG30) -I$(APPINC) -I$(DIR_ROOT) -o $@ $< 

$(OBJ_FILE) : %.o : $(DIR_ROOT)/%.c
	$(CC) -c $(CFLAGS_DEBUG20) -I$(APPINC) -I$(DIR_ROOT) -o $@ $< 

$(OBJ_GIF) : %.o : $(DIR_GIF)/%.c
	$(CC) -c $(CFLAGS_DEBUG30) -I$(APPINC) -I$(DIR_GIF) -o $@ $< 

$(OBJ_JPEG) : %.o : $(DIR_JPEG)/%.c
	$(CC) -c $(CFLAGS_DEBUG30) -I$(APPINC) -I$(DIR_JPEG) -o $@ $< 

$(OBJ_PNG) : %.o : $(DIR_PNG)/%.c
	$(CC) -c $(CFLAGS_DEBUG30) -I$(APPINC) -I$(DIR_PNG) -o $@ $< 


.PHONY : force clean

clean :
	$(RM-F) *.o
	$(RM-F) *.d
	$(RM-F) $(TARGET)

$(TARGET) : $(OBJS)
	$(AR) -rv $@ $(OBJS)
	cp $(TARGET) $(SDK_LIB)
