#!/usr/bin/env python
import sys
import time
import csv
from subprocess import run, Popen, DEVNULL

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

# Timing function for a single process
def timethis(command, thread_count, update_count, update_size, is_java):
	start = time.time()

	# Run the process
	if (is_java):
		run(["java", "-cp", "build/", process_name, str(thread_count), str(update_count), str(update_size)], stdout=DEVNULL)
	else:
		run(["build/" + command, str(thread_count), str(update_count), str(update_size)], stdout=DEVNULL)

	end = time.time()
	return int(round((end-start)*1000))

# Timing function for a multi-process
def batchtimethis(command, thread_count, update_count, update_size, addr):
	start = time.time()

	# Start server
	svr_proc = Popen(["build/" + command, str(thread_count), str(update_count), str(update_size), 's', addr])

	# Start all client one by one
	for i in range(0, thread_count):
		t = Popen(["build/" + command, str(thread_count), str(update_count), str(update_size), 'c', addr+"-"+str(i)])

	print("Waiting on server thread to finish")

	svr_proc.wait()

	# Wait for all to be finish
	# TODO : HOW TO
	end = time.time()
	return int(round((end-start)*1000))
	
# Method to write the result to the output
def writeRes(test_id, test_params, test_time, csvw):
	csvw.writerow([str(test_id), test_params, str(test_time)])
	if (test_id % 5 == 0):
		print("Test id:" + str(test_id))
	return
	
# Execute a INPROC test case
def executeInprocTestCase(command, is_java):
	# First thread count
	with open("output/" + command + "_th.csv", "w", newline="") as thcsv:
		csvwriter = csv.writer(thcsv, delimiter=';', quotechar='|', quoting=csv.QUOTE_MINIMAL)
		csvwriter.writerow(['test_id', 'test_param', 'test_time'])
		i = 0
		for tc in range(THREAD_START, THREAD_MAX, THREAD_STEP):
			i+=1
			writeRes(i, "TC=" + str(tc), timethis(command, tc, 1000, 10, is_java), csvwriter)
		writeRes(i+1, "TC=" + str(THREAD_MAX), timethis(command, THREAD_MAX, 1000, 10, is_java), csvwriter)

	# Update count
	with open("output/" + command + "_uc.csv", "w", newline="") as uccsv:
		csvwriter = csv.writer(uccsv, delimiter=';', quotechar='|', quoting=csv.QUOTE_MINIMAL)
		csvwriter.writerow(['test_id', 'test_param', 'test_time'])
		i = 0
		for uc in range(UPDATE_COUNT_START, UPDATE_COUNT_MAX, UPDATE_COUNT_STEP):
			i+=1
			writeRes(i, "UC=" + str(uc), timethis(command, 4, uc, 10, is_java), csvwriter)
		writeRes(i+1, "UC=" + str(UPDATE_COUNT_MAX), timethis(command, 4, UPDATE_COUNT_MAX, 10, is_java), csvwriter)

	# Update size TODO
	
	return

# Execute a IPC test case
def executeIpcTestCase(command, addr_start):
	# First thread count
	with open("output/" + command + "_th.csv", "w", newline="") as thcsv:
		csvwriter = csv.writer(thcsv, delimiter=';', quotechar='|', quoting=csv.QUOTE_MINIMAL)
		csvwriter.writerow(['test_id', 'test_param', 'test_time'])
		#i = 0
		#for tc in range(THREAD_START, THREAD_MAX, THREAD_STEP):
		#	i+=1
		#	writeRes(i, "TC=" + str(tc), batchtimethis(command, tc, 100, 10), csvwriter)
		writeRes(100, "TC=" + str(100), batchtimethis(command, 100, 100000, 10, addr_start + time.strftime("%Y%m%d-%H%M%S")), csvwriter)

	# Update count
	#with open("output/" + command + "_uc.csv", "w", newline="") as uccsv:
		#csvwriter = csv.writer(uccsv, delimiter=';', quotechar='|', quoting=csv.QUOTE_MINIMAL)
		#csvwriter.writerow(['test_id', 'test_param', 'test_time'])
		#i = 0
		#for uc in range(UPDATE_COUNT_START, UPDATE_COUNT_MAX, UPDATE_COUNT_STEP):
		#	i+=1
		#	writeRes(i, "UC=" + str(uc), batchtimethis(command, 4, uc, 10), csvwriter)
		#writeRes(i+1, "UC=" + str(UPDATE_COUNT_MAX), batchtimethis(command, 4, UPDATE_COUNT_MAX, 10), csvwriter)

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
	executeInprocTestCase("nano_inproc", False)
	print("== nano_inproc done ==")
	return

def java_inproc():
	print("== java_inproc starting ==")
	executeInprocTestCase("java_inproc", True)
	print("== java_inproc done ==")
	return

def pthread_inproc():
	print("== pthread_inproc starting ==")
	executeInprocTestCase("pthread_inproc", False)
	print("== pthread_inproc done ==")
	return

def ipc():
	nano_ipc()
	pipe_ipc()
	shared_ipc()
	return

def nano_ipc():
	print("== nano_ipc starting ==")
	executeIpcTestCase("nano_ipc", "ipc://nano_ipc_")
	print("== nano_ipc done ==")
	return

def pipe_ipc():
	print("== pipe_ipc starting ==")
	executeIpcTestCase("pipe_ipc", "/tmp/pipe_ipc_")
	print("== pipe_ipc done ==")
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

# Execute the target
if (torun in target.keys()):
	target[torun]()
	print("All done result in output/")
else:
	print("Target not found")