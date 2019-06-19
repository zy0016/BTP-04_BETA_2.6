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

ifeq ($(VERSION), RGB12)
BUILDDIR := ../src/rgb16_24
MACRO := LDD_MODE=DDM_RGB12_4440
endif

ifeq ($(VERSION), RGB16)
BUILDDIR := ../src/rgb16_24
MACRO := LDD_MODE=DDM_RGB16_565
endif

ifeq ($(VERSION), RGB24)
BUILDDIR := ../src/rgb16_24
MACRO := LDD_MODE=DDM_RGB24_BGR
endif

ifeq ($(VERSION), RGB32)
BUILDDIR := ../src/rgb16_24
MACRO := LDD_MODE=DDM_RGB32_BGR0
endif

TARGET = libdisplay.a

RM-F := rm -f

#Hopen SDK dirs
SDK_LIB = ../../../../../../lib
SDK_INCLUDE = ../../../../../include

INCPATH := $(SDK_INCLUDE) ../../../../include ../../src/include
SRCPATH := ../../src
COMMONSRCPATH := ../../src/common
SPECFILEPATH := ../../src/RGB
BUILDPATH := ../

CFLAGS_DEBUG = -g -c -O2 -Wall -nostdinc  -mcpu=arm920 -march=armv4 $(addprefix -I, $(INCPATH)) $(addprefix -D, $(MACRO)) -D _DEBUG
CFLAGS_RELEASE = -O2 -Wall  -mcpu=arm920 -march=armv4 -I$(addprefix -I, $(INCPATH)) $(addprefix -D, $(MACRO)) 

OBJS1 = display.o
OBJS2 = dispcomn.o disppdd.o
OBJS3 = rgb.o
OBJS4 = disp.o

OBJS = $(OBJS1) $(OBJS2) $(OBJS3) $(OBJS4)

all : $(TARGET)

$(OBJS1) : %.o : $(SRCPATH)/%.c
	$(CC) -c $(CFLAGS_DEBUG) $(addprefix -I, $(INCPATH))  -o $@ $< 
	
$(OBJS2) : %.o : $(COMMONSRCPATH)/%.c
	$(CC) -c $(CFLAGS_DEBUG) $(addprefix -I, $(INCPATH))  -o $@ $< 
	
$(OBJS3) : %.o : $(SPECFILEPATH)/%.c
	$(CC) -c $(CFLAGS_DEBUG) $(addprefix -I, $(INCPATH))  -o $@ $< 
	
$(OBJS4) : %.o : $(BUILDPATH)/%.c
	$(CC) -c $(CFLAGS_DEBUG) $(addprefix -I, $(INCPATH))  -o $@ $< 

.PHONY : force clean

clean :
	$(RM-F) *.o
	$(RM-F) *.d
	$(RM-F) libdisplay.a

$(TARGET) : $(OBJS)
	$(AR) -r $@ $(OBJS)
	cp $(TARGET)  $(SDK_LIB)
	$(RANLIB) $@
