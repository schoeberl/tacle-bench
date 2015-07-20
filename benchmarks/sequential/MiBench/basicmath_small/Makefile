
# Quick and dirty Makefile that assumes the benchmarks
# are cloned at the same level as the patmos repository

# We probably do not need all of them, but have them now for sure
CFLAGS?= -g -target patmos-unknown-unknown-elf -O2 \
		 -I../../../include \
        -mpatmos-disable-stack-cache \
        -mpatmos-disable-vliw \
        -mpatmos-method-cache-size=0x1000 -mpatmos-preferred-subfunction-size=0 \
        -mpatmos-stack-base=0x200000 -mpatmos-shadow-stack-base=0x1ff000

CFLAGS2?=-g -target patmos-unknown-unknown-elf -O2 \
	-I../../../include \
	-mpatmos-disable-vliw \
	-mpatmos-method-cache-size=0x1000 -mpatmos-preferred-subfunction-size=0 \
	-mpatmos-stack-base=0x200000 -mpatmos-shadow-stack-base=0x1f8000 


all: comp test

comp: basicmath-small-scache.elf basicmath-small.elf

basicmath-small.elf: *.c
	patmos-clang $(CFLAGS) *.c -o basicmath-small.elf

basicmath-small-scache.elf: *.c
	patmos-clang $(CFLAGS2) *.c -o basicmath-small-scache.elf

test: comp
	../../../../../patmos/hardware/build/paemu-2kwb-0ksc-2kmscdo basicmath-small.elf
	../../../../../patmos/hardware/build/paemu-2kwb-2ksc-0kmscdo basicmath-small.elf
	#pasim a.out
	patmos-emulator-2kbdmapwb basicmath-small.elf
	patmos-emulator-2kbdmapwbsc basicmath-small-scache.elf
	../../../../../patmos/hardware/build/emulator basicmath-small.elf
	patmos-emulator basicmath-small-scache.elf
	patmos-emulator basicmath-small.elf
