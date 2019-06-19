main:
	xscale-elf-gcc -g -c __gccmain.c -o __gccmain.o
	xscale-elf-gcc -g -c init.c -o init.o
	xscale-elf-ar -r libinit.a init.o __gccmain.o
	cp libinit.a ../libs

clean:
	rm *.o
	rm libinit.a
