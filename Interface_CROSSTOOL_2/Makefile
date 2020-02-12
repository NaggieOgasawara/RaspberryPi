CROSS= /home/naggie/cross/rpi/arm-unknown-eabi/bin/arm-unknown-eabi-
OBJECT= rpi-micon

#---------------------------------------
# ビルド release/debug切替
#---------------------------------------
buildtype := release
#buildtype := debug

CC= $(CROSS)gcc
AS= $(CROSS)as
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
MEMORYMAP= memory.x
OBJS= main.o

# RaspberryPi library
OBJS+= interrupt.o
OBJS+= io.o
OBJS+= pi.o
OBJS+= printf.o
OBJS+= vector.o
OBJS+= video2.o

#AFLAGS= -mcpu=arm1176jzf-s --defsym JTAG=1 -g
AFLAGS= -mcpu=arm1176jzf-s --defsym JTAG=0 -g
CFLAGS= -mcpu=arm1176jzf-s -mfloat-abi=soft
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

$(OBJECT).elf:	$(STARTUP) $(OBJS)
	$(LD) -T $(MEMORYMAP) -Map $(OBJECT).map $^ -o $@ $(LIBS) -lc -lgcc
	$(OBJDUMP) -S -D $(OBJECT).elf > $(OBJECT).disas
	$(SIZE) $(OBJECT).elf > $(OBJECT).size
	$(READELF) -a $(OBJECT).elf > $(OBJECT).readelf

.SUFFIXES: .elf .img

.elf.img:
	$(OBJCOPY) -O binary $< $@
.c.o:
	$(CC) $(CFLAGS) -c $< -o $@
.s.o:
	$(AS) $(AFLAGS) -c $< -o $@

clean::
	$(RM) -f *.o *.img *.elf */*.o */*/*.o
	$(RM) -f tags *~

tags::
	ctags *.[chS]
