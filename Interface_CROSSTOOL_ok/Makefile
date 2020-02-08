CROSS= /home/naggie/cross/rpi/arm-unknown-eabi/bin/arm-unknown-eabi-
#CROSS= arm-unknown-eabi-

#---------------------------------------
# ビルド release/debug切替
#---------------------------------------
buildtype := release
#buildtype := debug

CC= $(CROSS)gcc
LD= $(CROSS)ld
OBJCOPY= $(CROSS)objcopy
OBJDUMP= $(CROSS)objdump
SIZE= $(CROSS)size
READELF= $(CROSS)readelf

# libgcc.a
LIBS = -L/home/naggie/cross/rpi/arm-unknown-eabi/lib/gcc/arm-unknown-eabi/8.3.0/
# newlib
LIBS += -L/home/naggie/cross/rpi/arm-unknown-eabi/arm-unknown-eabi/lib/
STARTUP= crt0.o
OBJS= main.o

# RaspberryPi library
OBJS+= interrupt.o
OBJS+= io.o
OBJS+= pi.o
OBJS+= printf.o
OBJS+= vector.o

CFLAGS= -mcpu=arm1176jzf-s -mfloat-abi=soft 
#CFLAGS= -mcpu=arm1176jzf-s -mfloat-abi=soft -mlittle-endian
#CFLAGS+= -nostartfiles
# add start
#CFLAGS+= -fno-builtin
# add end
#CFLAGS+= -ffreestanding
# CFLAGS+= -marm -mcpu=arm1176jzf-s
CFLAGS+= -DRPI1
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


########################
all:	rpi-micon.img

rpi-micon.elf:	$(STARTUP) $(OBJS)
	$(LD) -static -nostartfiles -T memory.x $^ -o $@ $(LIBS) -lc -lgcc
	$(OBJDUMP) -D rpi-micon.elf > rpi-micon.disas
	$(SIZE) rpi-micon.elf > rpi-micon.size
	$(READELF) -a rpi-micon.elf > rpi-micon.readelf

.SUFFIXES: .elf .img

.elf.img:
	$(OBJCOPY) -O binary $< $@
.c.o:
	$(CC) $(CFLAGS) -c $< -o $@
.s.o:
	$(CC) $(CFLAGS) -c $< -o $@

clean::
	$(RM) -f *.o *.img *.elf */*.o */*/*.o
	$(RM) -f tags *~

tags::
	ctags *.[chS]
