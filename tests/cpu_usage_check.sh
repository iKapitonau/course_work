#!/bin/env bash
set -x
Ts=(0.001 0.005 0.01 0.03 0.05 0.07 0.1 0.2 0.3 0.4 0.5 0.6 0.7 0.8 0.9 1 1.5 2 2.5 3 3.5 4 4.5 5 6 7 8 9 10 11)
#rm -rf test.cpu.1.c
echo '============================= C ======================='
for i in ${Ts[@]}; do
	echo "T = $i"
	python CinPy.py get_cpu_useful 1 cpu $i c &
	sleep 1
	top -b -p $(jobs -p) -n 1 >> c_test.cpu-$i
	sleep 1
	kill $(jobs -p)
done

for i in c_test.cpu-*; do
	T=${i##*-}
	echo -n "$T " >> c.out
	awk $i '{total = total + $10} END {print total / 50}' >> c.out
done

echo '============================= Python ==================='
for i in ${Ts[@]}; do
	echo "T = $i"
	python CinPy.py get_cpu_useful 1 cpu $i py &
	sleep 1
	top -b -p $(jobs -p) -n 50 >> py_test.cpu-$i
	sleep 1
	kill $(jobs -p)
done

for i in py_test.cpu-*; do
	T=${i##*-}
	echo -n "$T " >> py.out
	awk $i '{total = total + $10} END {print total / 50}' >> py.out
done

gnuplot -p -e "set xlabel 'Время, сек'; set ylabel 'Загруженность процессора, log(%)'; set logscale y 2;  plot 'c.out' with lines title 'C', 'py.out' with lines title 'Python'"
