
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

comp: statemate-scache.elf statemate.elf

statemate.elf: *.c
	patmos-clang $(CFLAGS) *.c -o statemate.elf

statemate-scache.elf: *.c
	patmos-clang $(CFLAGS2) *.c -o statemate-scache.elf

test: comp
	../../../../../patmos/install/bin/paemu-2kwb-0ksc-2kmsc statemate.elf
	../../../../../patmos/install/bin/paemu-2kwb-0ksc-2kmscd statemate.elf
	../../../../../patmos/install/bin/paemu-2kwb-0ksc-2kmscdo statemate.elf
	../../../../../patmos/install/bin/paemu-2kwb-2ksc-0kmscdo statemate-scache.elf
	../../../../../patmos/install/bin/paemu-2kwt-0ksc-2kmscdo statemate.elf
	../../../../../patmos/install/bin/paemu-2kwt-2ksc-0kmscdo statemate-scache.elf
	../../../../../patmos/install/bin/paemu-4kwb-0ksc-0kmscdo statemate.elf
	../../../../../patmos/install/bin/paemu-4kwt-0ksc-0kmscdo statemate.elf
	#pasim a.out
