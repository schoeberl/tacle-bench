
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

comp: cjpeg_jpeg6b_wrbmp-scache.elf cjpeg_jpeg6b_wrbmp.elf

cjpeg_jpeg6b_wrbmp.elf: *.c
	patmos-clang $(CFLAGS) *.c -o cjpeg_jpeg6b_wrbmp.elf

cjpeg_jpeg6b_wrbmp-scache.elf: *.c
	patmos-clang $(CFLAGS2) *.c -o cjpeg_jpeg6b_wrbmp-scache.elf

test: comp
	../../../../../patmos/install/bin/paemu-2kwb-0ksc-2kmsc cjpeg_jpeg6b_wrbmp.elf
	../../../../../patmos/install/bin/paemu-2kwb-0ksc-2kmscd cjpeg_jpeg6b_wrbmp.elf
	../../../../../patmos/install/bin/paemu-2kwb-0ksc-2kmscdo cjpeg_jpeg6b_wrbmp.elf
	../../../../../patmos/install/bin/paemu-2kwb-2ksc-0kmscdo cjpeg_jpeg6b_wrbmp-scache.elf
	../../../../../patmos/install/bin/paemu-2kwt-0ksc-2kmscdo cjpeg_jpeg6b_wrbmp.elf
	../../../../../patmos/install/bin/paemu-2kwt-2ksc-0kmscdo cjpeg_jpeg6b_wrbmp-scache.elf
	../../../../../patmos/install/bin/paemu-4kwb-0ksc-0kmscdo cjpeg_jpeg6b_wrbmp.elf
	../../../../../patmos/install/bin/paemu-4kwt-0ksc-0kmscdo cjpeg_jpeg6b_wrbmp.elf
	#pasim a.out
