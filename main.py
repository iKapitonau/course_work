#!/usr/bin/env python
# coding: utf-8

# In[1]:


import pandas as pd
import numpy as np
from sklearn.neighbors import LocalOutlierFactor
import time
import scan


# In[8]:


def prepare(prev_data, data):
    diff_cpu_total = data[0] + data[1] + data[2] - prev_data[0] - prev_data[1] - prev_data[2]
    diff_cpu_useful = data[0] - prev_data[0]
    cpu_usage = diff_cpu_useful / diff_cpu_total
    mem_ram_free = data[6] / data[5]
    mem_swap_free = data[4] / data[3]
    disk_opcount = data[7]
    diff_disk_iotime = data[8] - prev_data[8]
    diff_cpu_iowait = data[2] - prev_data[2]
    return [cpu_usage, diff_cpu_iowait, mem_ram_free, mem_swap_free, disk_opcount, diff_disk_iotime]


# In[9]:


def get_dataset(filename):
    raw_data = pd.read_csv(filename).values
    X = np.zeros(shape=(len(raw_data),6))
    for i in range(1, len(raw_data)):
        X[i - 1] = np.array(prepare(raw_data[i - 1], raw_data[i]))
    return X


# In[10]:


def draw_graph(X):
    X_norm = (X - X.min())/(X.max() - X.min())

    from sklearn.decomposition import PCA as sklearnPCA
    import matplotlib.pyplot as plt

    pca = sklearnPCA(n_components=2) #2-dimensional PCA
    transformed = pd.DataFrame(pca.fit_transform(X_norm))
    plt.scatter(transformed[0], transformed[1], c='red')
    plt.show()


# In[2]:


if __name__ == '__main__':
    T = 6
    scan.scan_all(3 * 60 * 60, 6, 'mem_runtime')
    X = get_dataset('training_dataset')
    #draw_graph(X)
    lof = LocalOutlierFactor(novelty=True)
    lof.fit(X)
    prev_data = scan.scan_once()
    time.sleep(T)
    with open('dataset_runtime', 'a') as f:
        while True:
            data = scan.scan_once()
            verdict = lof.predict(np.array([prepare(prev_data, data)]))
            for i in data:
                f.write('%i,' % i)
            f.write('%i\n' % verdict)
            prev_data = data
            time.sleep(T)

