# make file made by wb33
USER_LIB_PATH = ..\lib
ROOT_DIR = ..\..
# macro definitions for tools & dir

GCC33 = gcc33
PP33  = pp33
EXT33 = ext33
AS33  = as33
LK33  = lk33
LIB33 = lib33
MAKE  = make
SRC_DIR = 

# macro definitions for tool flags

GCC33_FLAG = -S -O -fno-builtin -I..\..\..\sysinc -I..\..\..\include -I..\..\src\include
PP33_FLAG  = -g 
EXT33_FLAG = 
AS33_FLAG  = -g 
LK33_FLAG  = -g -s -m -c 
EXT33_CMX_FLAG = -lk test -c 
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
SRC1_DIR=$(ROOT_DIR)\rgb\src
DISP_SRC_DIR = $(ROOT_DIR)\src
DISP_INCLUDE = $(DISP_SRC_DIR)\include
DISP_COMMON_DIR = $(DISP_SRC_DIR)\common
DISP_RGBCOM_DIR = $(DISP_SRC_DIR)\rgbcom
DISP_MONOCOM_DIR = $(DISP_SRC_DIR)\monocom

DISPLAY_OBJ = display.o
COMMON_OBJ = dispcomn.o disppdd.o

DISPLAY_MS = display.ms
COMMON_MS = dispcomn.ms disppdd.ms

TARGET = display.lib

#*************************************************************
#special define start

DISP_DISPLAY_DIR = $(DISP_SRC_DIR)\MONO_RGB
DISP_MONO_DIR = $(DISP_DISPLAY_DIR)\MONO
DISP_RGB_DIR = $(DISP_DISPLAY_DIR)\RGB

RGB_OBJ =  rgb.o
MONO_OBJ = mono.o
OBJS    = $(COMMON_OBJ) $(DISPLAY_OBJ) $(RGB_OBJ) $(MONO_OBJ)

SPEC_DISPLAY_FLAG = -I$(DISP_INCLUDE)
SPEC_RGB_FLAG = -I$(DISP_INCLUDE) -I$(DISP_RGBCOM_DIR)
SPEC_MONO_FLAG = -I$(DISP_INCLUDE) -I$(DISP_MONOCOM_DIR)

#SPEC_MACRO = DDM_RGB12_4440
#SPEC_MACRO = DDM_RGB12_4440
#SPEC_MACRO = DDM_RGB16_565
#SPEC_MACRO = DDM_RGB24_BGR
#SPEC_MACRO = DDM_RGB18_BGR0
#SPEC_MACRO = DDM_RGB32_BGR0

#special define end
#*************************************************************
#*************************************************************
#special files to be compiled star

$(TARGET) : $(OBJS) 
	$(LIB33) $(LIB33_FLAG) $(TARGET) $(OBJS)
	copy $(TARGET) $(USER_LIB_PATH)\$(TARGET)
	del *.lib

## display.c
display.ms : $(DISP_DISPLAY_DIR)\display.c
	$(GCC33) $(GCC33_FLAG) $(SPEC_DISPLAY_FLAG) -DLDD_MODE=$(SPEC_MACRO) $(DISP_DISPLAY_DIR)\$*.c
	$(EXT33) $(EXT33_FLAG) $*.ps
display.o : display.ms

## rgb.c
rgb.ms : $(DISP_RGB_DIR)\rgb.c
	$(GCC33) $(GCC33_FLAG) $(SPEC_RGB_FLAG) -DLDD_MODE=$(SPEC_MACRO) $(DISP_RGB_DIR)\$*.c
	$(EXT33) $(EXT33_FLAG) $*.ps
rgb.o : rgb.ms

## mono.c
mono.ms : $(DISP_MONO_DIR)\mono.c
	$(GCC33) $(GCC33_FLAG) $(SPEC_MONO_FLAG) $(DISP_MONO_DIR)\$*.c
	$(EXT33) $(EXT33_FLAG) $*.ps
mono.o : mono.ms

#special files to be compiled end
#************************************************************

## dispcomn.c
dispcomn.ms : $(DISP_COMMON_DIR)\dispcomn.c
	$(GCC33) $(GCC33_FLAG) $(DISP_COMMON_DIR)\$*.c
	$(EXT33) $(EXT33_FLAG) $*.ps
dispcomn.o : dispcomn.ms

## disppdd.c
disppdd.ms : $(DISP_COMMON_DIR)\disppdd.c
	$(GCC33) $(GCC33_FLAG) $(DISP_COMMON_DIR)\$*.c
	$(EXT33) $(EXT33_FLAG) $*.ps
disppdd.o : disppdd.ms

### src definition end

# dependency list end

# optimaization by 2 pass make

opt:
	$(MAKE) -f test.mak
	cwait 2
	$(EXT33) $(EXT33_CMX_FLAG) test.cmx
	$(MAKE) -f test.mak

# clean files except source

clean:
	del *.o
	del *.ms
	del *.ps
