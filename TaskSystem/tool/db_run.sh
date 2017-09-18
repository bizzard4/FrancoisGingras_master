#!/bin/bash

echo "Running multiple databas client Nmax=$1 Req=$2 ReqType=$3"



echo "Running code ->"
for i in $(seq 1 $1)
do
	echo "$i/$1"
	echo "db_out_$1$i.txt"
	test/db_client_test $2 $3 > db_out_$1$i.txt &
done

wait

echo "Done"