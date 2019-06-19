CP = cp
TARGET = libdisplay.a
DSTPATH = ../lib
all:
	@echo @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	@echo     This make is for display module. Writed by WuWeihua.
	@echo     Usage:
	@echo         make [target]
	@echo     Target:
	@echo         rgb:  
	@echo				Support single display mode.Special the macro
	@echo             SPEC_MACRO in the rgb.mak.
	@echo         mono_rgb:
	@echo             Support double display modes, one is mono and 
	@echo             the other is color. Special the macro 
	@echo             SPEC_MACRO in the mono_rgb.mak.
	@echo         mono_rgb12_rgb16: 
	@echo             Support three display modes, a mono, a rgb12
	@echo             and a rgb16.
	@echo         rgb12_rgb18: 
	@echo             Support double display modes, one is rgb12 and 
	@echo             the other is rgb18. 
	@echo         rgb16_rgb18: 
	@echo             Support double display modes, one is rgb16 and 
	@echo             the other is rgb18. 	
	@echo         rgb_rgb:
	@echo             Support two same display mode.Special the macro
	@echo             SPEC_MACRO in the rgb_rgb.mak.
	@echo     Example:
	@echo             make rgb
	@echo             make mono_rgb12_rgb16
	@echo @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
alltarget:
	make clean
	make rgb12
	make rgb16
	make rgb24
	make rgb32
	make mono_rgb16
	make mono_rgb32
	make rgb12_rgb18
	make rgb16_rgb18
	make mono_rgb12_rgb16
	make rgb16_rgb16
	make clean

rgb12:
	@echo @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	@echo     Use the rgb.mak as the makefile. 
	@echo @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	make -f rgb.mak SPEC_MACRO = DDM_RGB12_4440
	$(CP) $(TARGET) $(DSTPATH)/libdisplay_mx1_rgb16.a

rgb16:
	@echo @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	@echo     Use the rgb.mak as the makefile. 
	@echo @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	make -f rgb.mak SPEC_MACRO=DDM_RGB16_565
	$(CP) $(TARGET) ../../../../../../src_ads/libs/libdisplay.a

rgb24:
	@echo @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	@echo     Use the rgb.mak as the makefile. 
	@echo @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	make -f rgb.mak SPEC_MACRO=DDM_RGB24_BGR
	$(CP) $(TARGET) $(DSTPATH)/libdisplay_mx1_rgb24bgr.a

rgb32:
	@echo @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	@echo     Use the rgb.mak as the makefile. 
	@echo @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	make -f rgb.mak SPEC_MACRO=DDM_RGB18_BGR0
	$(CP) $(TARGET) $(DSTPATH)/libdisplay_mx1_rgb32bgr0.a

mono_rgb16:
	@echo @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	@echo     Use the mono_rgb.mak as the makefile. 
	@echo @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	make -f mono_rgb.mak SPEC_MACRO=DDM_RGB16_565
	$(CP) $(TARGET) $(DSTPATH)/libdisplay_mx1_1_rgb16.a

mono_rgb32:
	@echo @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	@echo     Use the mono_rgb.mak as the makefile. 
	@echo @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	make -f mono_rgb.mak SPEC_MACRO=DDM_RGB18_BGR0
	$(CP) $(TARGET) $(DSTPATH)/libdisplay_mx1_1_rgb32bgr0.a

rgb12_rgb18:
	@echo @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	@echo     Use the rgb12_rgb18.mak as the makefile. 
	@echo @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	make -f rgb12_rgb18.mak
	$(CP) $(TARGET) $(DSTPATH)/libdisplay_mx1_12_32bgr0.a
	
rgb16_rgb18:
	@echo @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	@echo     Use the rgb16_rgb18.mak as the makefile. 
	@echo @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	make -f rgb16_rgb18.mak
	$(CP) $(TARGET) $(DSTPATH)/libdisplay_mx1_16_32bgr0.a

mono_rgb12_rgb16:
	@echo @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	@echo     Use the mono_rgb12_rgb16.mak as the makefile. 
	@echo @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	make -f mono_rgb12_rgb16.mak
	$(CP) $(TARGET) $(DSTPATH)/libdisplay_mx1_1_12_16.a

rgb16_rgb16:
	@echo @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	@echo     Use the rgb_rgb.mak as the makefile. 
	@echo @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	make -f rgb_rgb.mak SPEC_MACRO=DDM_RGB16_565
	$(CP) $(TARGET) $(DSTPATH)/libdisplay_mx1_16_16.a



# clean files except source

clean:
	rm -f *.o
	rm -f libdisplay.a
	rm -f *.d
