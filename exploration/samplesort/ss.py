import threading
import copy
import asyncio
import math
import random

# Sample sort implementation in Python for the purpose of understanding the concept

# Input :
# 1) Array of n elements
# 2) k tasks (generate them)

# Output :
# Sorted array of n elements

# Algorithm (from Todd email)

# Global variable (need lock and reset)
root_lock = None
root_sample = None
root_sample_received = None
root_sample_event = None
threads = None

# Represent a task (bucket) for the sample sort
class ss_task(threading.Thread):
	def __init__(self, threadID, name, sample, N, SN, K, global_lock, barrier_lock):
		threading.Thread.__init__(self)
		self.threadID = threadID
		self.name = name
		self.sample = sample
		self.N = N
		self.SN = SN
		self.K = K
		self.global_lock = global_lock
		self.barrier_lock = barrier_lock
		self.splitters = None
		self.data = []
		self.data_lock = threading.Lock()

	def run(self):
		global root_sample, root_sample_received, root_sample_event, threads

		# Sort data (nlogn)
		self.sample = sorted(self.sample)
		print("STARTING", self.name, "-", self.sample)

		# Gather sample and send them to the main method (using lock)
		step = int(self.SN / self.K)
		sample_to_send = []
		print("SAMPLING", self.name, "-",  step)
		for ni in range(0, self.SN, step):
			sample_to_send.append(self.sample[ni])		

		self.global_lock.acquire()
		print(root_sample_received, root_sample)
		root_sample.extend(sample_to_send)
		root_sample_received += 1
		if (root_sample_received == self.K):
			root_sample_event.set()
		self.global_lock.release()

		# Wait on splitters information
		print("Barrier waiting with", self.barrier_lock.n_waiting, "others")
		self.barrier_lock.wait()

		# Go throught all data and send them at the right task
		print("SWAPPING", self.name, "-", self.splitters)
		for val in self.sample:
			splitter_size = len(self.splitters)
			for i in range(1, splitter_size):
				if val < self.splitters[i]:
					print(self.name, "sending", val, "to task", i-1)
					threads[i-1].insertValue(val)
					break
				elif i == splitter_size-1:
					print(self.name, "sending", val, "to task", i)
					threads[i].insertValue(val)

	# In final version, this would do a insertion sort, but here we will just do append and sorted
	def insertValue(self, newVal):
		self.data_lock.acquire()
		self.data.append(newVal)
		self.data = sorted(self.data)
		self.data_lock.release()

	def setSplitter(self, splitters):
		print(self.name, "- Splitters = ", splitters)
		self.splitters = splitters

# Return sorted array (copy of)
def samplesort(data, N, K):
	global root_sample, root_sample_received, root_sample_event, threads

	print("Full data", data)

	# Reset global data
	root_sample = []
	root_lock = threading.Lock()
	root_sample_received = 0
	root_sample_event = threading.Event()
	threads = [None]*K

	try:
		#size_per_task = round(N/K) # Rounding (do not floor)
		size_per_task = int(N/K)
		barrier = threading.Barrier(K+1)

		current_index = 0
		for ki in range(K):
			data_to_task = copy.deepcopy(data[current_index:current_index+size_per_task])

			# Add rest of the array to the last one
			if ki == K-1:
				data_to_task.extend(copy.deepcopy(data[current_index+size_per_task:]))

			threads[ki] = ss_task(ki, "Task " + str(ki), data_to_task, N, size_per_task, K, root_lock, barrier)
			threads[ki].start()
			current_index += size_per_task

		# Wait for all sample from all task (event synchronization)
		root_sample_event.wait()
		print("Got all sample", root_sample)

		# Compute and send splitters info to all task
		root_sample = sorted(root_sample)
		step = int(len(root_sample) / K)
		print("Splitters step", step)
		splitters = []
		for ni in range(0, len(root_sample), step):
			splitters.append(root_sample[ni])	
		for ki in range(K):
			threads[ki].setSplitter(splitters)

		# Unlock barrier, so all thread can start swapping (barrier synchronization)
		barrier.wait()

		# Wait for all thread to be done
		for ki in range(K):
			threads[ki].join()

		print("Done - final data")
		for ki in range(K):
			print("Task", ki, threads[ki].data)
	except Exception as e:
		print("Error while creating threads", e)

# Test input

input_1 = [2, 5, 8, 6, 3, 1, 7, 9, 10, 4]
input_1_n = len(input_1) #10
k_1 = 2

input_2 = [2, 5, 8, 6, 3, 1, 7, 9, 10, 4]
input_2_n = len(input_1) #10
k_2 = 3

input_3 = []
for i in range(19):
	input_3.append(i)
input_3_n = len(input_3)
k_3 = 4

input_4 = []
for i in range(200000):
	input_4.append(random.randrange(10000))
input_4_n = 200000
k_4 = 59


# Test run

samplesort(input_1, input_1_n, k_1)
print()
samplesort(input_2, input_2_n, k_2)
print()
samplesort(input_3, input_3_n, k_3)
print()
samplesort(input_4, input_4_n, k_4)