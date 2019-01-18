#! /bin/bash

i=1
while(($i<23))
do
#	echo "i= $i"
	python newLVCfg_GEMROC_TIGER.py $i 0
	let "i++"
done
