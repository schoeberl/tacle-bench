
# Quick and dirty Makefile that assumes the benchmarks
# are cloned at the same level as the patmos repository

# We probably do not need all of them, but have them now for sure
CFLAGS?= -g -target patmos-unknown-unknown-elf -O2 \
        -mpatmos-disable-stack-cache \
        -mpatmos-disable-vliw \
        -mpatmos-method-cache-size=0x1000 -mpatmos-preferred-subfunction-size=0 \
        -mpatmos-stack-base=0x200000 -mpatmos-shadow-stack-base=0x1ff000

CFLAGS2?=-g -target patmos-unknown-unknown-elf -O2 \
	-mpatmos-disable-vliw \
	-mpatmos-method-cache-size=0x1000 -mpatmos-preferred-subfunction-size=0 \
	-mpatmos-stack-base=0x200000 -mpatmos-shadow-stack-base=0x1f8000 

all: comp test

comp: anagram-scache.elf anagram.elf

anagram.elf: *.c
	patmos-clang $(CFLAGS) *.c -o anagram.elf

anagram-scache.elf: *.c
	patmos-clang $(CFLAGS2) *.c -o anagram-scache.elf

test: comp
	#pasim a.out
	patmos-emulator-2kbdmapwb anagram.elf
	patmos-emulator-2kbdmapwbsc anagram-scache.elf
	../../../../../patmos/hardware/build/emulator anagram.elf
	patmos-emulator anagram-scache.elf
