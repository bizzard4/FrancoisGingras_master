#!/bin/bash

echo "Starting samplesort timing K=$1 N=$2 dataset=$3 runs=$4"

echo "Header generation ->"
python3 ../tool/timing_header.py $1 > k$1_n$2.csv

echo "Running code ->"
for n in $(seq 1 $4)
do
	echo "Running $n/$4"
	test/samplesort_test $1 $2 < $3 > out_k$1_n$2_$n.txt
done

echo "Aggregation ->"
for ni in $(seq 1 $4)
do
	python3 ../tool/parse_timing.py $1 out_k$1_n$2_$ni.txt >> k$1_n$2.csv
done