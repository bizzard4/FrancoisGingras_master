# Makefile to build benchmarking binaries

# Compiler
CC = gcc
CFLAGS = -g -Wall
JC = javac
JFLAGS = -g

# Include folder
INCLUDES = -I/usr/local/include/nanomsg -Icommon

# Lib folder
LFLAGS = -L/usr/local/lib

# Libraries
LIBS = -lnanomsg -lpthread

# Targets
# all -> inproc, ipc, tcp

default: all

all: inproc ipc tcp

inproc: common nano_inproc java_inproc pthread_inproc

ipc: common nano_ipc pipe_ipc shared_ipc dns_ipc

tcp: common nano_tcp socket_tcp

nano_inproc:
	@echo Building nano_inproc
	$(CC) $(CFLAGS) $(INCLUDES) inproc/nano_inproc/nano_inproc.c -c -o build/nano_inproc.o $(LFLAGS) $(LIBS)
	$(CC) $(CFLAGS) $(INCLUDES) build/nano_inproc.o build/list.o -o build/nano_inproc $(LFLAGS) $(LIBS)

java_inproc:
	@echo Building java_inproc
	$(JC) $(JFLAGS) inproc/java_inproc/java_inproc.java -d build/

pthread_inproc:
	@echo Building pthread_inproc
	$(CC) $(CFLAGS) $(INCLUDES) inproc/pthread_inproc/pthread_inproc.c -c -o build/pthread_inproc.o $(LFLAGS) $(LIBS)
	$(CC) $(CFLAGS) $(INCLUDES) build/pthread_inproc.o build/list.o -o build/pthread_inproc $(LFLAGS) $(LIBS)

nano_ipc:
	@echo Building nano_ipc
	$(CC) $(CFLAGS) $(INCLUDES) ipc/nano_ipc/nano_ipc.c -c -o build/nano_ipc.o $(LFLAGS) $(LIBS)
	$(CC) $(CFLAGS) $(INCLUDES) build/nano_ipc.o build/list.o -o build/nano_ipc $(LFLAGS) $(LIBS)

pipe_ipc:
	@echo Building pipe_ipc
	$(CC) $(CFLAGS) $(INCLUDES) ipc/pipe_ipc/pipe_ipc.c -c -o build/pipe_ipc.o $(LFLAGS) $(LIBS)
	$(CC) $(CFLAGS) $(INCLUDES) build/pipe_ipc.o build/list.o -o build/pipe_ipc $(LFLAGS) $(LIBS)

shared_ipc:
	@echo Building shared_ipc
	$(CC) $(CFLAGS) $(INCLUDES) ipc/shared_ipc/shared_ipc.c -c -o build/shared_ipc.o $(LFLAGS) $(LIBS)
	$(CC) $(CFLAGS) $(INCLUDES) build/shared_ipc.o build/list.o -o build/shared_ipc $(LFLAGS) $(LIBS)

dns_ipc:
	@echo Building dns_ipc
	$(CC) $(CFLAGS) $(INCLUDES) ipc/dns_ipc/dns_ipc.c -c -o build/dns_ipc.o $(LFLAGS) $(LIBS)
	$(CC) $(CFLAGS) $(INCLUDES) build/dns_ipc.o build/list.o -o build/dns_ipc $(LFLAGS) $(LIBS)

nano_tcp:
	@echo Building nano_tcp
	$(CC) $(CFLAGS) $(INCLUDES) tcp/nano_tcp/nano_tcp.c -c -o build/nano_tcp.o $(LFLAGS) $(LIBS)
	$(CC) $(CFLAGS) $(INCLUDES) build/nano_tcp.o build/list.o -o build/nano_tcp $(LFLAGS) $(LIBS)

socket_tcp:
	@echo Building socket_tcp
	$(CC) $(CFLAGS) $(INCLUDES) tcp/socket_tcp/socket_tcp.c -c -o build/socket_tcp.o $(LFLAGS) $(LIBS)
	$(CC) $(CFLAGS) $(INCLUDES) build/socket_tcp.o build/list.o -o build/socket_tcp $(LFLAGS) $(LIBS)

.PHONY: common
common: 
	@echo Building common
	$(CC) $(CFLAGS) $(INCLUDES) common/list.c -c -o build/list.o $(LFLAGS)

clean:
	@echo Cleaning build folder
	rm -f build/*
