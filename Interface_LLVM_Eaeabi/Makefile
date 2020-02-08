#************************************************************************/
#*      GCC/LLCM ARM Baremetal Software / (C) Toshio MURAI  2012-2016   */
#*      This is Free Software.   You can redistribute this software     */
#*      and/or modify it ,under the terms of the GNU General Public     */
#*      License version 3, or (at your option) any later version.       */
#*      or under the terms of the BSD 3-Clause License.	(Dual Licence)  */
#************************************************************************/
CPU=arm1176jzf-s
#	ARCH=armv6zk-a

#---------------------------------------
# ビルド release/debug切替
#---------------------------------------
buildtype := release
#buildtype := debug

#CPATH=/usr/local/bin/arm-eabi-
CPATH=/home/naggie/cross/rpi/arm-unknown-eabi/bin/arm-unknown-eabi-
CC=$(CPATH)gcc
AS=$(CPATH)as
LD=$(CPATH)ld
OBJCOPY=$(CPATH)objcopy
OBJDUMP= $(CPATH)objdump
SIZE= $(CPATH)size
READELF= $(CPATH)readelf

OBJECTS=vector.o crt0.o main.o io.o interrupt.o pi.o printf.o
#OBJECTS=vector.o crt0.o main.o io.o interrupt.o pi.o printf.o matrix.o fonts.o index.o
MEMORYMAP=memory.x
OBJECTNAME=RPI

# ifeq ($(CC), clang)
# 	LIBGCC=/home/user/arm/lib/libcortexa
# else
# 	LIBGCC=/usr/local/lib/gcc/arm-eabi/5.3.0/
# endif
# 	LIBC=/usr/local/arm-eabi/lib
# libgcc.a
LIBS = -L/home/naggie/cross/rpi/arm-unknown-eabi/lib/gcc/arm-unknown-eabi/8.3.0/
# newlib
LIBS += -L/home/naggie/cross/rpi/arm-unknown-eabi/arm-unknown-eabi/lib/


#---------------------------------------
# ビルド release/debug切替
#---------------------------------------
ifeq ($(buildtype),release)
        CFLAGS += -O2
else ifeq ($(buildtype),debug)
        CFLAGS += -O0 -g -gdwarf-2
else
        $(error buildtype must be release, debug, profile or coverage)
endif

# ifeq ($(CC), clang)
# 	CFLAGS= -target arm-eabi -fshort-enums
# else
# 	CFLAGS=
# endif
	CFLAGS+= -mcpu=$(CPU) -msoft-float -g -O2 -DRPI1
# 	CFLAGS+= -I. -I.. -I/usr/local/include
	CFLAGS+= -I. -I..
	ASFLAGS= -mcpu=$(CPU) --defsym JTAG=1 # -mfpu=neon

#	CFLAGS= -mcpu=$(CPU) -mtune=$(CPU) -march=$(ARCH)	gcc
#	CFLAGS= -target arm-eabi -mcpu=$(CPU) -mtune=$(CPU)	clang

$(OBJECTNAME).out:  $(OBJECTS)
#	$(LD) -T $(MEMORYMAP) -Map $(OBJECTNAME).map $(OBJECTS) -L$(LIBGCC) -L$(LIBC) -lgcc -lc -o $(OBJECTNAME).out
	$(LD) -T $(MEMORYMAP) -Map $(OBJECTNAME).map $(OBJECTS) $(LIBS) -o $(OBJECTNAME).out
	$(OBJCOPY) -O srec $(OBJECTNAME).out $(OBJECTNAME).srec
	$(OBJCOPY) -O ihex $(OBJECTNAME).out $(OBJECTNAME).hex
	$(OBJCOPY) -O binary $(OBJECTNAME).out $(OBJECTNAME).bin
	$(OBJCOPY) -O binary $(OBJECTNAME).out kernel.img

crt0.o	:	crt0.s
	$(AS) $(ASFLAGS) -g  	$< -o	$@

vector.o	:	vector.c
	$(CC) $(CFLAGS) -c	$<	-o $@

main.o	:	main.c io.h
	$(CC) $(CFLAGS) -c	$<	-o $@

printf.o	:	printf.c printf.h
	$(CC) $(CFLAGS) -c	$<	-o $@

pi.o	:	pi.c
	$(CC) $(CFLAGS) -c	$<	-o $@

io.o	:	io.c io.h
	$(CC) $(CFLAGS) -c	$<	-o $@

interrupt.o: interrupt.c
	$(CC) $(CFLAGS) -c	$<	-o $@

clean	:
	rm -rf *.o $(OBJECTNAME).* kernel*.img
