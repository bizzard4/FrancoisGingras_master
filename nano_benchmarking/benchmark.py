#!/usr/bin/env python
import sys
import time
import subprocess

# Constants
THREAD_START = 2
THREAD_MAX = 100
THREAD_STEP = 2

UPDATE_COUNT_START = 10
UPDATE_COUNT_MAX = 1000000
UPDATE_COUNT_STEP = 1000

UPDATE_SIZE_START = 10 # To implement
UPDATE_SIZE_MAX = 20 # To implement
UPDATE_SIZE_STEP = 1 # To implement

torun = sys.argv[1];
print("Benchmarking for " + torun)

# Timing function
def timethis(command, thread_count, update_count, update_size):
	start = time.time()
	subprocess.run([command, str(thread_count), str(update_count), str(update_size)])
	end = time.time()
	return int(round((end-start)*1000))

# Creating benchmark target function
def all():
	inproc()
	ipc()
	tcp()
	return

def inproc():
	nano_inproc()
	java_inproc()
	pthread_inproc()
	return

def nano_inproc():
	print("== nano_inproc starting ==")
	# First thread count
	for tc in range(THREAD_START, THREAD_MAX, THREAD_STEP):
		print(timethis("build/nano_inproc", tc, 1000, 10));
	print(timethis("build/nano_inproc", THREAD_MAX, 1000, 10));

	# Update count

	# Update size TODO
	print("== nano_inproc done ==")
	return

def java_inproc():
	return

def pthread_inproc():
	return

def ipc():
	nano_ipc()
	pipe_ipc()
	shared_ipc()
	return

def nano_ipc():
	return

def pipe_ipc():
	return

def shared_ipc():
	return

def tcp():
	nano_tcp()
	socket_tcp()
	return

def nano_tcp():
	return

def socket_tcp():
	return

# Function dictionary
target ={
	"all": all,
	"inproc": inproc,
	"nano_inproc": nano_inproc,
	"pthread_inproc": pthread_inproc,
	"java_inproc": java_inproc,
	"ipc": ipc,
	"nano_ipc": nano_ipc,
	"pipe_ipc": pipe_ipc,
	"shared_ipc": shared_ipc,
	"tcp": tcp,
	"nano_tcp": nano_tcp,
	"socket_tcp": socket_tcp
}

# Execute benchmarking
if (torun in target.keys()):
	target[torun]()
	print("All done result in output/")
else:
	print("Target not found")