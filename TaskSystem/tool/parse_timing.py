import sys

# Return time in s from the string
def getTime(str_time):
	split = str_time.split(',')
	sec_str = split[0]
	ms_str = split[1]

	sec = int(sec_str[:len(sec_str)-1])
	ms = int(ms_str[:len(ms_str)-2])
	return sec + (ms/1000)


if len(sys.argv) != 3:
	print("Usage : K output_file")
	sys.exit()

K = int(sys.argv[1])

#TIMING-SAMPLESORT 0s,0ms - Initialization time 
ss_it = 0
#TIMING-SAMPLESORT 0s,0ms - Sample random sampling time 
ss_rs = 0
#TIMING-SAMPLESORT 0s,0ms - Sample bucket spawn time
ss_bs = 0
#TIMING-SAMPLESORT 12s,285ms - Final waiting on bucket time 
ss_wt = 0

#TIMING-TEST 11s,280ms - Read time
t_rt = 0
#TIMING-TEST 0s,0ms - Task initialization time
t_it = 0
#TIMING-TEST 12s,285ms - Task wait on done time
t_wt = 0

buckets = {}
#TIMING-BUCKET 2 0s,0ms - Get sample time                'b_sam_t'
#TIMING-BUCKET 2 0s,0ms - Waiting on splitter time       'b_sp_t'
#TIMING-BUCKET 2 1s,304ms - Propagation time             'b_prop_t'
#TIMING-BUCKET 2 0s,654ms - Rebuild array time           'b_rebuild_t'
#TIMING-BUCKET 2 10s,326ms - Final sorting time          'b_sort_t'
#TIMING-BUCKET 2 0s,0ms - Send time accumulator          'b_send_acc'
#TIMING-BUCKET 2 0s,334ms - Receive waiting accumulator  'b_recv_acc'
#TIMING-BUCKET 2 43655600 - Reveived values              'b_val_count'

file = open(sys.argv[2])
for line in file:
	splitted = line.split(' ')
	if line.startswith("TIMING-TEST"): # Test timing
		time = getTime(splitted[1])
		if "Read time" in line:
			t_rt = time
		elif "Task initialization" in line:
			t_it = time
		elif "Task wait" in line:
			t_wt = time
	elif line.startswith("TIMING-SAMPLESORT"): # Samplesort task timing (root)
		time = getTime(splitted[1])
		if "Initialization" in line:
			ss_it = time
		elif "Sample random" in line:
			ss_rs = time
		elif "Sample bucket" in line:
			ss_bs = time
		elif "Final waiting" in line:
			ss_wt = time
	elif line.startswith("TIMING-BUCKET"): # Bucket task timing
		b_id = int(splitted[1])
		if b_id not in buckets.keys():
			buckets[b_id] = {}
		if "Reveived values" in line:
			buckets[b_id]['b_val_count'] = int(splitted[2])
		else:
			time = getTime(splitted[2])
			if "Get sample time" in line:
				buckets[b_id]['b_sam_t'] = time
			elif "Waiting on splitter time" in line:
				buckets[b_id]['b_sp_t'] = time
			elif "Propagation" in line:
				buckets[b_id]['b_prop_t'] = time
			elif "Rebuild" in line:
				buckets[b_id]['b_rebuild_t'] = time
			elif "Final sorting" in line:
				buckets[b_id]['b_sort_t'] = time
			elif "Send time" in line:
				buckets[b_id]['b_send_acc'] = time
			elif "Receive waiting" in line:
				buckets[b_id]['b_recv_acc'] = time

# Print to stdout the csv style timing, all in second
print(*[t_rt, t_it, t_wt, ss_it, ss_rs, ss_bs, ss_wt], sep=';', end=';')
for ki in range(2, K+2):
	print(*[buckets[ki]['b_sam_t'], buckets[ki]['b_sp_t'], buckets[ki]['b_prop_t'], buckets[ki]['b_rebuild_t'], buckets[ki]['b_sort_t'], buckets[ki]['b_send_acc'], buckets[ki]['b_recv_acc'], buckets[ki]['b_val_count']], sep=';', end=';')
print()