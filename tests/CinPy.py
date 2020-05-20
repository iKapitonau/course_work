#!/usr/bin/env python
# coding: utf-8

# In[8]:


from ctypes import *
import sys
import time
import scan


# In[23]:


so_file = './scand.so'
cfunc = CDLL(so_file)
c_method = getattr(cfunc, sys.argv[1])
py_method = getattr(scan, sys.argv[1])
c_total = 0
py_total = 0
if sys.argv[3] == 'time':
	for i in range(int(sys.argv[2])):
		c_start = time.time()
		c_method()
		c_end = time.time()
		py_start = time.time()
		py_method()
		py_end = time.time()
		c_total += c_end - c_start
		py_total += py_end - py_start
		print('%d\t%.6f\t%.6f' % (i, (c_end-c_start) * 100000, (py_end - py_start) * 100000))
	print('Total:\t%.6f\t%.6f' % (c_total * 100000, py_total * 100000))
elif sys.argv[3] == 'mem':
	for i in range(int(sys.argv[2])):
		if sys.argv[4] == 'c':
			c_method()
		elif sys.argv[4] == 'py':
			py_method()
elif sys.argv[3] == 'cpu':
	DELAY = float(sys.argv[4])
	if sys.argv[5] == 'c':
		while True:
			cfunc.get_cpu_useful()
			cfunc.get_cpu_idle()
			cfunc.get_cpu_iowait()
			cfunc.get_mem_swap()
			cfunc.get_mem_ram()
			cfunc.get_disk_opcount()
			cfunc.get_disk_iotime()
			time.sleep(DELAY)
	else:
		while True:
			scan.get_cpu_useful()
			scan.get_cpu_idle()
			scan.get_cpu_iowait()
			scan.get_mem_swap()
			scan.get_mem_ram()
			scan.get_disk_opcount()
			scan.get_disk_iotime()
			time.sleep(DELAY)
