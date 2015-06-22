
# Quick and dirty Makefile that assumes the benchmarks
# are cloned at the same level as the patmos repository

# We probably do not need all of them, but have them now for sure
CFLAGS?= -g -target patmos-unknown-unknown-elf -O2 \
        -mpatmos-disable-stack-cache \
        -mpatmos-disable-vliw \
        -mpatmos-method-cache-size=0x1000 -mpatmos-preferred-subfunction-size=0 \
        -mpatmos-stack-base=0x200000 -mpatmos-shadow-stack-base=0x1ff000

CFLAGS2?=-target patmos-unknown-unknown-elf -O2 \
	-mpatmos-disable-vliw \
	-mpatmos-method-cache-size=0x1000 -mpatmos-preferred-subfunction-size=0 \
	-mpatmos-stack-base=0x200000 -mpatmos-shadow-stack-base=0x1f8000 

all: comp test

comp: dijkstra.elf dijkstra-scache.elf

dijkstra.elf: *.c
	patmos-clang $(CFLAGS) *.c -o dijkstra.elf

dijkstra-scache.elf: *.c
	patmos-clang $(CFLAGS2) *.c -o dijkstra-scache.elf

test: comp
	#pasim a.out
	patmos-emulator-2kbdmapwb dijkstra.elf
	../../../../../patmos/hardware/build/emulator dijkstra.elf
