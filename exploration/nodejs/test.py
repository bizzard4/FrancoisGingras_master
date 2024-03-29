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

# Timing function for a multi-process
def batchtimethis(thread_count, update_count, update_size):
	start = time.time()

	# Start server
	svr_proc = Popen(["node", "server.js", str(thread_count), str(update_count), str(update_size)])
	time.sleep(1);
	# Start all client one by one
	for i in range(0, thread_count):
		t = Popen(["node", "client.js", str(thread_count), str(update_count), str(update_size)])

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

# Execute a IPC test case
def executeIpcTestCase():
	# First thread count
	with open("test.csv", "w", newline="") as thcsv:
		csvwriter = csv.writer(thcsv, delimiter=';', quotechar='|', quoting=csv.QUOTE_MINIMAL)
		csvwriter.writerow(['test_id', 'test_param', 'test_time'])
		#i = 0
		#for tc in range(THREAD_START, THREAD_MAX, THREAD_STEP):
		#	i+=1
		#	writeRes(i, "TC=" + str(tc), batchtimethis(command, tc, 100, 10), csvwriter)
		writeRes(1, "TC=" + str(1), batchtimethis(1, 100, 10), csvwriter)

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

print("== nodejs_tcp starting ==")
executeIpcTestCase()
print("== nodejc_tcp done ==")