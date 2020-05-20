#!/bin/env bash

set -x

n=$1
funcs=('get_cpu_useful' 'get_cpu_idle' 'get_cpu_iowait' 'get_mem_swap_free' 'get_mem_ram_free' 'get_disk_opcount' 'get_disk_iotime')

for i in ${funcs[@]}; do
	echo $i >> test.time
	python CinPy.py $i $n time >> test.time
done

echo ==================== C ======================= >> test.mem
for i in ${funcs[@]}; do
	echo $i >> test.mem
	valgrind python3 CinPy.py $i $n mem c 2>&1 | grep -A 3 'HEAP SUMMARY'  >> test.mem
done

echo ==================== Python ======================= >> test.mem
for i in ${funcs[@]}; do
	echo $i >> test.mem
	valgrind python3 CinPy.py $i $n mem py  2>&1 | grep -A 3 'HEAP SUMMARY'  >> test.mem
done

bash cpu_usage_check.sh
