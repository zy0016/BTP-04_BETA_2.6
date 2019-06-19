CC = m68k-elf-gcc
AR = m68k-elf-ar
RANLIB = m68k-elf-ar -s
LD = m68k-elf-gcc

BUILDDIR := ..

TARGET 		:= libwin.a
#ST_TARGET 	:= libwinst.a
#MT_TARGET	:= libwinmt.a

RM-F := rm -f

INCPATH := . ../../../../include ../src/include ../src ../../../include

CFLAGS_ST = -g -c -O2 -Wall -nostdinc -m68000 -msoft-float $(addprefix -I, $(INCPATH)) $(addprefix -D, $(MACRO))  -D _DEBUG -D __ST_HOPEN_GUI=1
CFLAGS_ST_STD= -O2 -Wall -m68000 -msoft-float -I$(addprefix -I, $(INCPATH)) $(addprefix -D, $(MACRO)) -D __ST_HOPEN_GUI=1

CFLAGS_MT = -g -c -O2 -Wall -nostdinc -m68000 -msoft-float $(addprefix -I, $(INCPATH)) $(addprefix -D, $(MACRO)) -D _DEBUG -D __MT_HOPEN_GUI=1
CFLAGS_MT_STD= -O2 -Wall -m68000 -msoft-float -I$(addprefix -I, $(INCPATH)) $(addprefix -D, $(MACRO)) -D __MT_HOPEN_GUI=1

CFLAGS = $(CFLAGS_ST)

ifeq ($(VERSION), MT)
CFLAGS = $(CFLAGS_MT)
endif


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
OBJ_UI      = apidlg.o apimenu.o apiscrol.o button.o combobox.o commdlg.o \
              defdlg.o defwnd.o listbox.o menuobj.o menuwnd.o mledit.o \
              msgbox.o scroll.o spin.o static.o wsui.o 
OBJ_FONTDRV = fontdrv.o
OBJ_HGOS    = osal.o filesys.o inputmgr.o 
OBJ_MAIN    = wsmain.o
OBJ_MISC    = memory.o wstime.o

OBJS        = $(OBJ_KERNEL) $(OBJ_UI) $(OBJ_FONTDRV) $(OBJ_HGOS) $(OBJ_MAIN) $(OBJ_MISC)

DIR_KERNEL  = ../src/kernel
DIR_UI      = ../src/ui
DIR_FONTDRV = ../src/fontdrv
DIR_HGOS    = ../src/hgos
DIR_MAIN    = ../src/main
DIR_MISC    = ../src/misc



all : $(TARGET)

$(OBJ_MAIN) : %.o : $(DIR_MAIN)/%.c
	$(CC) -c $(CFLAGS) -I$(APPINC) -I$(DIR_MAIN) -o $@ $< 

$(OBJ_KERNEL) : %.o : $(DIR_KERNEL)/%.c
	$(CC) -c $(CFLAGS) -I$(APPINC) -I$(DIR_KERNEL) $(addprefix -I, $(INCPATH)) -I$(DIR_KERNEL) -o $@ $< 

$(OBJ_UI) : %.o : $(DIR_UI)/%.c
	$(CC) -c $(CFLAGS) -I$(APPINC) -I$(DIR_UI) -o $@ $< 

$(OBJ_FONTDRV) : %.o : $(DIR_FONTDRV)/%.c
	$(CC) -c $(CFLAGS) -I$(APPINC) -I$(DIR_FONTDRV) -o $@ $< 

$(OBJ_HGOS) : %.o : $(DIR_HGOS)/%.c
	$(CC) -c $(CFLAGS) -I$(APPINC) -I$(DIR_HGOS) -o $@ $< 

$(OBJ_MISC) : %.o : $(DIR_MISC)/%.c
	$(CC) -c $(CFLAGS) -I$(APPINC) -I$(DIR_MISC) -o $@ $< 
	
.PHONY : force clean

st : $(ST_TARGET)
mt : $(MT_TARGET)

everything : $(TARGET)
clean :
	$(RM-F) *.o
	$(RM-F) *.d
	$(RM-F) $(TARGET)

$(TARGET) : $(OBJS)
	$(AR) -rv $@ $(OBJS)

$(ST_TARGET): $(OBJS)
	$(AR) -rv $@ $(OBJS)
	
$(MT_TARGET): $(OBJS)
	$(AR) -rv $@ $(OBJS)	