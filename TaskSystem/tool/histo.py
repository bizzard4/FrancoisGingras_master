import sys

# Script to generate the histogram of a given set of data. Used to validate randomness of the dataset.
# Usage : python3 histo.py < input_file > output.csv

print("v;counts")

d = {}

for line in sys.stdin:
	val = int(line)
	if val not in d.keys():
		d[val] = 0

	d[val] += 1


for k in d.keys():
	print(k, d[k], sep=';')