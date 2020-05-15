#!/usr/bin/env python
# coding: utf-8

# In[19]:


def get_cpu_useful():
    res = 0
    with open('/proc/stat', 'r') as f:
        for i, t in enumerate(f.read().split()[1:11]):
            if i != 3 and i != 4:
                res += int(t)
    return res


# In[21]:


def get_cpu_idle():
    with open('/proc/stat', 'r') as f:
        return int(f.read().split()[4])


# In[24]:


def get_cpu_iowait():
    with open('/proc/stat', 'r') as f:
        return int(f.read().split()[5])


# In[50]:


def get_mem_swap():
    total, free = 0, 0
    cnt = 0
    with open('/proc/meminfo', 'r') as f:
        for i in f.read().split('\n'):
            if i.startswith('SwapTotal'):
                total = int(i.split()[1])
                cnt += 1
            if i.startswith('SwapFree'):
                free = int(i.split()[1])
                cnt += 1
            if cnt == 2:
                break
    return free / total


# In[53]:


def get_mem_ram():
    total, avail = 0, 0
    cnt = 0
    with open('/proc/meminfo', 'r') as f:
        for i in f.read().split('\n'):
            if i.startswith('MemTotal'):
                total = int(i.split()[1])
                cnt += 1
            if i.startswith('MemAvailable'):
                avail = int(i.split()[1])
                cnt += 1
            if cnt == 2:
                break
    return avail / total


# In[62]:


def get_disk_opcount():
    res = 0
    with open('/proc/diskstats', 'r') as f:
        for i in f.read().split('\n'):
            if i != '':
                res += int(i.split()[11])
    return res


# In[69]:


def get_disk_iotime():
    res = 0
    with open('/proc/diskstats', 'r') as f:
        for i in f.read().split('\n'):
            if i != '':
                print(i.split()[12])
                res += int(i.split()[12])
    return res


# In[ ]:




