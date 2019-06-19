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

MACRO := __ST_HOPEN_GUI=1

ifeq ($(VERSION), MT)
MACRO := __MT_HOPEN_GUI=1
endif

ifeq ($(VERSION), XMT)
MACRO := __XMT_HOPEN_GUI=1
endif

ifeq ($(VERSION), MP)
MACRO := __MP_HOPEN_GUI=1
endif

TARGET 		:= libwin.a
#ST_TARGET 	:= libwinst.a
#MT_TARGET	:= libwinmt.a

RM-F := rm -f

#Hopen SDK dirs
SDK_LIB = ../../../../../lib
SDK_INCLUDE = ../../../../include

SRC_PATH = ../../src
INCLUDE_PATH = $(SRC_PATH)/include 
INCPATH = $(SDK_INCLUDE) $(INCLUDE_PATH) $(SRC_PATH) ../../../include

CFLAGS_DEBUG = -g -c -O2 -Wall -nostdinc -mcpu=arm920 -march=armv4 $(addprefix -I, $(INCPATH)) $(addprefix -D, $(MACRO)) -D_DEBUG
CFLAGS_RELEASE = -O2 -Wall -mcpu=arm920 -march=armv4 -I$(addprefix -I, $(INCPATH)) $(addprefix -D, $(MACRO))

OBJ_API     = apigdi.o wsapi.o apirgn.o 
OBJ_WIN     = clstbl.o defwndbs.o sysmetri.o winclass.o \
              wincoord.o winexpos.o winmap.o winmem.o winobj.o winpaint.o \
              winpos.o winroot.o wsgraph.o wsinit.o 
OBJ_GDI     = devdisp.o devfont.o graphdev.o wsobmp.o wsobrush.o \
              wsocur.o wsodc.o wsofont.o wsopen.o wsorgn.o wsotable.o
OBJ_MSG     = devinput.o devqueue.o message.o msgqueue.o
OBJ_OTHER   = error.o rectmgr.o resmgr.o wscaret.o wsclipbo.o wscursor.o \
              wshook.o wstimer.o wsthread.o

OBJ_KERNEL  = $(OBJ_API) $(OBJ_WIN) $(OBJ_GDI) $(OBJ_MSG) $(OBJ_OTHER)
OBJ_UI      = apidlg.o apimenu.o apiscrol.o button.o combobox.o \
              defdlg.o defwnd.o listbox.o menuobj.o menuwnd.o mledit.o \
              msgbox.o scroll.o strspin.o numspin.o static.o wsui.o 
OBJ_FONTDRV = fontdrv.o
OBJ_HGOS    = osal.o filesys.o inputmgr.o 
OBJ_MAIN    = wsmain.o
OBJ_MISC    = memory.o wstime.o codepage.o
OBJ_MOBILE  = winpda.o

OBJS        = $(OBJ_KERNEL) $(OBJ_UI) $(OBJ_FONTDRV) $(OBJ_HGOS) $(OBJ_MAIN) $(OBJ_MISC) $(OBJ_MOBILE)

DIR_KERNEL  = ../../src/kernel
DIR_UI      = ../../src/ui
DIR_FONTDRV = ../../src/fontdrv
DIR_HGOS    = ../../src/hgos
DIR_MAIN    = ../../src/main
DIR_MISC    = ../../src/misc
DIR_MOBILE  = ../../src/mobile/xscale



all : $(TARGET)

$(OBJ_MAIN) : %.o : $(DIR_MAIN)/%.c
	$(CC) -c $(CFLAGS_DEBUG) -I$(APPINC) -I$(DIR_MAIN) -o $@ $< 

$(OBJ_KERNEL) : %.o : $(DIR_KERNEL)/%.c
	$(CC) -c $(CFLAGS_DEBUG) -I$(APPINC) -I$(DIR_KERNEL) -o $@ $< 

$(OBJ_UI) : %.o : $(DIR_UI)/%.c
	$(CC) -c $(CFLAGS_DEBUG) -I$(APPINC) -I$(DIR_UI) -o $@ $< 

$(OBJ_FONTDRV) : %.o : $(DIR_FONTDRV)/%.c
	$(CC) -c $(CFLAGS_DEBUG) -I$(APPINC) -I$(DIR_FONTDRV) -o $@ $< 

$(OBJ_HGOS) : %.o : $(DIR_HGOS)/%.c
	$(CC) -c $(CFLAGS_DEBUG) -I$(APPINC) -I$(DIR_HGOS) -o $@ $< 

$(OBJ_MISC) : %.o : $(DIR_MISC)/%.c
	$(CC) -c $(CFLAGS_DEBUG) -I$(APPINC) -I$(DIR_MISC) -o $@ $< 
	
$(OBJ_MOBILE) : %.o : $(DIR_MOBILE)/%.c
	$(CC) -c $(CFLAGS_DEBUG) -I$(APPINC) -I$(DIR_MOBILE) -o $@ $< 	
.PHONY : force clean

clean :
	$(RM-F) *.o
	$(RM-F) *.d
	$(RM-F) $(TARGET)

$(TARGET) : $(OBJS)
	$(AR) -rv $@ $(OBJS)
	cp $(TARGET)  $(SDK_LIB)
$(ST_TARGET): $(OBJS)
	$(AR) -rv $@ $(OBJS)
	
$(MT_TARGET): $(OBJS)
	$(AR) -rv $@ $(OBJS)	