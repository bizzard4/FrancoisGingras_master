#!/bin/bash

# Generate random value using 500k shunk
# Command : shuf -i 1-500000 -n N
# Usage : gen.sh N

SHUNK_COUNT=$(($1 / 500000))
for ((i=0; i < $SHUNK_COUNT; i++)); do
	shuf -i 1-500000 -n 500000
done

SHUNK_REST=$(($1 % 500000))
shuf -i 1-500000 -n $SHUNK_REST