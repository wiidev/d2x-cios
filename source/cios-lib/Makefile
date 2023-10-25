# devkitARM path
DEVKITARM ?=	/opt/devkitARM

# Prefix
PREFIX	=	$(DEVKITARM)/bin/arm-eabi-

# Executables
CC	=	$(PREFIX)gcc
AR	=	$(PREFIX)ar

# Flags
ARCH	=	-mcpu=arm926ej-s -mthumb -mthumb-interwork -mbig-endian
CFLAGS	=	$(ARCH) -I. -ffreestanding -fomit-frame-pointer -Os -Wall
CFLAGS2	=	-Wstrict-prototypes -ffunction-sections
ARFLAGS	=	rcs

# Libraries
LIBS	=

# Target
TARGET	=	cios-lib

# Objects
OBJS	=	direct_syscalls.o	\
		fat_tools.o		\
		ios.o			\
		ipc.o			\
		isfs.o			\
		libc.o			\
		log.o			\
		mem.o			\
		sdhc_server.o	\
		sdio.o			\
		stealth.o		\
		str_utils.o		\
		swi_mload.o		\
		syscalls.o		\
		syscalls_dbg.o	\
		timer.o			\
		tools.o			\
		usbstorage.o	\
		vsprintf.o

OBJS2	=	libwbfs/libwbfs.o	\
		libwbfs/rijndael.o	\
		libwbfs/wiidisc.o


$(TARGET).a: $(OBJS) $(OBJS2)
	@echo -e " AR\t$@"
	@$(AR) $(ARFLAGS) $@ $(OBJS) $(OBJS2)

%.o: %.s
	@echo -e " CC\t$@"
	@$(CC) $(CFLAGS) -D_LANGUAGE_ASSEMBLY -c -x assembler-with-cpp -o $@ $<

libwbfs/%.o: libwbfs/%.c
	@echo -e " CC\t$@"
	@$(CC) $(CFLAGS) $(CFLAGS2) -c -o $@ $<

%.o: %.c
	@echo -e " CC\t$@"
	@$(CC) $(CFLAGS) -c -o $@ $<

clean:
	@echo -e "Cleaning..."
	@rm -f $(OBJS) $(OBJS2) $(TARGET).a
