import sys

print("Samplesort - Validatiaon")

if len(sys.argv) != 5:
	print("Usage : K N sample_output_file data_file")
	sys.exit()

K = int(sys.argv[1])
N = int(sys.argv[2])

buckets_val = {}
splitters = []

sorted_input = []
# Read the input, and sort it
input_file = open(sys.argv[4])
for line in input_file:
	sorted_input.append(int(line))
sorted_input = sorted(sorted_input)
#print(sorted_input)

# Starting to read the output file
output_file = open(sys.argv[3])
for line in output_file:
	if line.startswith("Splitter :"): # Parse the splitter
		print("Splitter line :", line)
		splitted = line.split(" ")
		for ki in range(2, K+1): # Get the k-1 splitters
			s = splitted[ki]
			splitters.append(int(s[s.index("v=")+2:len(s)-1]))
		splitters = sorted(splitters)
		print("Splitter found -", splitters)
		splitters.append(1000000000) # Last splitter is ignored
	elif line.startswith("BUCKET="):
		splitted = line.split(" ")
		b_id = int(splitted[0][7:])
		val = int(splitted[1][6:])
		if b_id not in buckets_val.keys():
			buckets_val[b_id] = []
		buckets_val[b_id].append(val)

# Validation
count = 0
for ki in range(2, K+2):
	for val in buckets_val[ki]:
		if val != sorted_input[count]:
			print("Val=", val, "Sorted val=", sorted_input[count])
			print("FINAL ORDER ERROR")
			exit()
		else: # Val is at the right place, not validate splitter
			if val > splitters[ki-2]:
				print("Val=", val, "Splitters=", splitters[ki-2])
				print("SPLITTER ERROR")
				exit()
			else:
				count += 1

print("SUCCESS")

