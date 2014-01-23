# Makefile for Hello World project

TARGET = control
CC = gcc
LDFLAGS = -lncurses

SRC = $(TARGET).c
#SRC += utils.c

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
