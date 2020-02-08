CROSS= arm-unknown-eabi-

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
STARTUP= startup.o
OBJS= main.o

# systemcall
#OBJS += syscalls.o

# RaspberryPi library
OBJS+= rpi_lib/rpi_init.o
OBJS+= rpi_lib/gpio/gpio.o
OBJS+= rpi_lib/timer/timer.o
OBJS+= rpi_lib/delay/delay.o
OBJS+= rpi_lib/serial/serial.o
OBJS+= rpi_lib/video/video.o
OBJS+= rpi_lib/bss/clearbss.o

CFLAGS= -mfloat-abi=soft -mlittle-endian
CFLAGS+= -nostartfiles
# add start
CFLAGS+= -fno-builtin
# add end
CFLAGS+= -ffreestanding
CFLAGS+= -O2
# CFLAGS+= -marm -mcpu=arm1176jzf-s


########################
all:	rpi-micon.img

rpi-micon.elf:	$(STARTUP) $(OBJS)
	$(LD) -static -nostartfiles -T rpiMicon.lds $^ -o $@ $(LIBS) -lc -lgcc
	$(OBJDUMP) -D rpi-micon.elf > rpi-micon.disas
	$(SIZE) rpi-micon.elf > rpi-micon.size
	$(READELF) -a rpi-micon.elf > rpi-micon.readelf

.SUFFIXES: .elf .img

.elf.img:
	$(OBJCOPY) -O binary $< $@
.c.o:
	$(CC) $(CFLAGS) -c $< -o $@
.S.o:
	$(CC) $(CFLAGS) -c $< -o $@

clean::
	$(RM) -f *.o *.img *.elf */*.o */*/*.o
	$(RM) -f tags *~

tags::
	ctags *.[chS]
