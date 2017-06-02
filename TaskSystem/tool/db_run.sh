#!/bin/bash

echo "Running multiple databas client N=$1 Req=$2"



echo "Running code ->"
for n in $(seq 1 $1)
do
	test/db_client_test $2 > db_out_$n.txt &
done

wait

echo "Done"