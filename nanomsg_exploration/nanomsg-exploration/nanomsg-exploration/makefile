#Makefile nanomsg benchmarking

# Compiler
CC = gcc
CFLAGS = -g -Wall

# Include folder
INCLUDES = -I/usr/local/include/nanomsg

# File to compiole
CFILES = list.c main.c 

# Lib folder
LFLAGS = -L/usr/local/lib

# Libraries
LIBS = -lnanomsg -lpthread

# Targets
TARGET = explo

default: all

all: $(TARGET)

$(TARGET): 
	$(CC) $(CFLAGS) $(INCLUDES) $(CFILES) -o build/out $(LFLAGS) $(LIBS)


# Example command that work
# gcc -I../../nanomsg-1.0.0/src list.c main.c -o test -L/usr/local/lib -lnanomsg

