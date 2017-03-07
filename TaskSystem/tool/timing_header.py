import sys

if len(sys.argv) != 2:
	print("Usage : K")
	sys.exit()

K = int(sys.argv[1])

#TIMING-TEST 11s,280ms - Read time - t_rt 
#TIMING-TEST 0s,0ms - Task initialization time - t_it
#TIMING-TEST 12s,285ms - Task wait on done time - t_wt

#TIMING-SAMPLESORT 0s,0ms - Initialization time  - ss_it
#TIMING-SAMPLESORT 0s,0ms - Sample random sampling time - ss_rs
#TIMING-SAMPLESORT 0s,0ms - Sample bucket spawn time - ss_bs
#TIMING-SAMPLESORT 12s,285ms - Final waiting on bucket time - ss_wt

#TIMING-BUCKET 2 0s,0ms - Get sample time                'b_sam_t'
#TIMING-BUCKET 2 0s,0ms - Waiting on splitter time       'b_sp_t'
#TIMING-BUCKET 2 1s,304ms - Propagation time             'b_prop_t'
#TIMING-BUCKET 2 0s,654ms - Rebuild array time           'b_rebuild_t'
#TIMING-BUCKET 2 10s,326ms - Final sorting time          'b_sort_t'
#TIMING-BUCKET 2 0s,0ms - Send time accumulator          'b_send_acc'
#TIMING-BUCKET 2 0s,334ms - Receive waiting accumulator  'b_recv_acc'
#TIMING-BUCKET 2 43655600 - Reveived values              'b_val_count'

arr = [
	't_read_time', 
	't_init_time', 
	't_wait_time', 
	'ss_init_time', 
	'ss_sampling_time', 
	'ss_spawn_time', 
	'ss_wait_time']

print(*arr, sep=';', end=';')
for ki in range(2, K+2):
	b_arr = [
		'b' + str(ki) + '_get_sample_time',
		'b' + str(ki) + '_get_splitters_time',
		'b' + str(ki) + '_propagation_time',
		'b' + str(ki) + '_rebuild_time',
		'b' + str(ki) + '_sort_time',
		'b' + str(ki) + '_send_time',
		'b' + str(ki) + '_recv_time',
		'b' + str(ki) + '_val_count']
	print(*b_arr, sep=';', end=';')
print()

