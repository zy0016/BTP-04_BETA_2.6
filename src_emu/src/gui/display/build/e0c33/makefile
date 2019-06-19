all:
	@echo ***********************************************************
	@echo     This make is for display module. 
	@echo     Usage:
	@echo         make [target]
	@echo     Target:
	@echo         rgb:  
	@echo             Support single display mode.Special the macro
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
	@echo     Example:
	@echo             make rgb
	@echo             make mono_rgb12_rgb16
	@echo ***********************************************************

rgb:
	@echo ***********************************************************
	@echo     Use the rgb.mak as the makefile. 
	@echo     You should define the SPEC_MACRO in the rgb.mak.
	@echo ***********************************************************
	make -f rgb.mak

mono_rgb:
	@echo ***********************************************************
	@echo     Use the mono_rgb.mak as the makefile. 
	@echo     You should define the SPEC_MACRO in the mono_rgb.mak 
	@echo     to decide the rgb color depth.
	@echo ***********************************************************
	make -f mono_rgb.mak

rgb12_rgb18:
	@echo ***********************************************************
	@echo     Use the rgb12_rgb18.mak as the makefile. 
	@echo ***********************************************************
	make -f rgb12_rgb18.mak

mono_rgb12_rgb16:
	@echo ***********************************************************
	@echo     Use the mono_rgb12_rgb16.mak as the makefile. 
	@echo ***********************************************************
	make -f mono_rgb12_rgb16.mak

# clean files except source

clean:
	del *.o
	del *.ms
	del *.ps
