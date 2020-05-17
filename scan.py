from ctypes import *
import time

T = 3

if __name__ == '__main__':
	libname = './scand.so'
	c_lib = CDLL(libname)

	print('cpu_useful', 'cpu_idle', 'cpu_iowait', 'mem_swap_total', 'mem_swap_free', 'mem_ram_total',
		'mem_ram_free', 'disk_opcount', 'disk_iotime', sep=',')
	while True:
		print(c_lib.get_cpu_useful(), c_lib.get_cpu_idle(), c_lib.get_cpu_iowait(),
			c_lib.get_mem_swap_total(), c_lib.get_mem_swap_free(),
			c_lib.get_mem_ram_total(), c_lib.get_mem_ram_free(),
			c_lib.get_disk_opcount(), c_lib.get_disk_iotime(), sep=',')
		time.sleep(T)
