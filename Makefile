# Makefile for Hello World project

TARGET = control
CC = gcc
#CC = /home/dlinyj/tplink/wrt/staging_dir/toolchain-mips_34kc_gcc-4.8-linaro_uClibc-0.9.33.2/bin/mips-openwrt-linux-gcc
#CC = mips-openwrt-linux-gcc
LDFLAGS = -lpthread
LDFLAGS += -D_REENTERANT
#LDFLAGS += -lncurses

SRC = $(TARGET).c
SRC += mpc.c
SRC += encoder.c
SRC += display.c

OBJ = $(SRC:.c=.o)

all: $(TARGET)
	
$(TARGET): obj
	#$(CC) -Wall -o $(TARGET) -g $(OBJ) $(LDFLAGS)
	$(CC) -o $(TARGET) -g $(OBJ) $(LDFLAGS)
	#strip $(TARGET)

obj: $(SRC)
	$(CC) -Wall -c -g $(SRC) 
clean:
	rm -f *.o $(TARGET) 
