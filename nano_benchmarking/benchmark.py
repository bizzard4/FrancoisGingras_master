#!/usr/bin/env python
import sys
import time
import subprocess

# Constants
THREAD_START = 2
THREAD_MAX = 100 # ~50 runs
THREAD_STEP = 2

UPDATE_COUNT_START = 10
UPDATE_COUNT_MAX = 1000000 # ~200 runs
UPDATE_COUNT_STEP = 5000

UPDATE_SIZE_START = 10 # To implement
UPDATE_SIZE_MAX = 20 # To implement
UPDATE_SIZE_STEP = 1 # To implement

torun = sys.argv[1];
print("Benchmarking for " + torun)

# Timing function for the java program
def javaTime(process_name, thread_count, update_count, update_size):
	start = time.time()
	subprocess.run(["java", "-cp", "build/", process_name, str(thread_count), str(update_count), str(update_size)])
	end = time.time()
	return int(round((end-start)*1000))
	
# Timing function for a native program
def processTime(command, thread_count, update_count, update_size):
	start = time.time()
	subprocess.run([command, str(thread_count), str(update_count), str(update_size)])
	end = time.time()
	return int(round((end-start)*1000))

# Timing function
def timethis(command, thread_count, update_count, update_size, is_java):
	if (is_java):
		return javaTime(command, thread_count, update_count, update_size)
	else:
		return processTime(command, thread_count, update_count, update_size)
	
# Method to write the result to the output
def writeRes(test_id, test_params, test_time):
	print(str(test_id) + ":" + " " + test_params + " time=" + str(test_time) + "ms")
	return
	
def executeTestCase(command, is_java):
	# First thread count
	i = 0
	for tc in range(THREAD_START, THREAD_MAX, THREAD_STEP):
		i+=1
		writeRes(i, "TC=" + str(tc), timethis(command, tc, 1000, 10, is_java))
	writeRes(i, "TC=" + str(THREAD_MAX), timethis(command, THREAD_MAX, 1000, 10, is_java))

	# Update count
	i = 0
	for uc in range(UPDATE_COUNT_START, UPDATE_COUNT_MAX, UPDATE_COUNT_STEP):
		i+=1
		writeRes(i, "UC=" + str(uc), timethis(command, 4, uc, 10, is_java))
	writeRes(i, "UC=" + str(UPDATE_COUNT_MAX), timethis(command, 4, UPDATE_COUNT_MAX, 10, is_java))

	# Update size TODO
	
	return

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
	executeTestCase("build/nano_inproc", False)
	print("== nano_inproc done ==")
	return

def java_inproc():
	print("== java_inproc starting ==")
	executeTestCase("java_inproc", True)
	print("== java_inproc done ==")
	return

def pthread_inproc():
	print("== pthread_inproc starting ==")
	executeTestCase("build/pthread_inproc", False)
	print("== pthread_inproc done ==")
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