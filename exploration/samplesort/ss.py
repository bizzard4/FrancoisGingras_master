import threading
import copy

# Sample sort implementation in Python for the purpose of understanding the concept

# Input :
# 1) Array of n elements
# 2) k tasks (generate them)

# Output :
# Sorted array of n elements

# Algorithm (from Todd email)

class ss_task(threading.Thread):
	def __init__(self, threadID, name, data, N, K):
		threading.Thread.__init__(self)
		self.threadID = threadID
		self.name = name
		self.data = data
		self.N = N
		self.K = K

	def run(self):
		#print("Starting " + self.name)
		print(self.data)
		self.data = sorted(self.data)
		print(self.data)
		#print("Exiting " + self.name)



input_1 = [2, 5, 8, 6, 3, 1, 7, 9, 10, 4]
input_1_n = len(input_1) #10
k_1 = 2

input_2 = [2, 5, 8, 6, 3, 1, 7, 9, 10, 4]
input_2_n = len(input_1) #10
k_2 = 3

# Return sorted array (copy of)
def samplesort(data, N, K):
	print("Full data", data)
	try:
		size_per_task = int(N/K) # Floor
		threads = [None]*K
		current_index = 0
		for ki in range(K):
			data_to_task = copy.deepcopy(data[current_index:current_index+size_per_task])

			if ki == K-1:
				data_to_task.extend(copy.deepcopy(data[current_index+size_per_task:])) # Rest of the array

			threads[ki] = ss_task(ki, "Task " + str(ki), data_to_task)
			threads[ki].start()
			current_index += size_per_task

		for ki in range(K):
			threads[ki].join()

		print("Done")
	except Exception as e:
		print("Error while creating threads", e)

samplesort(input_1, input_1_n, k_1)
print()
samplesort(input_2, input_2_n, k_2)