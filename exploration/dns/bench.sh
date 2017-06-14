#!/bin/bash

echo "Running multiple dns client N=$1 Req=$2"



echo "Running code ->"
for n in $(seq 1 $1)
do
	./client $2 > out_$n.txt &
done

wait

echo "Done"