import yfinance, random
from tqdm import tqdm
import pandas as pd
import numpy as np
import threading

class YF_grabber(threading.Thread):
    def __init__(self, sec_code, sec_name, lock_group):
        threading.Thread.__init__(self)
        self.code = sec_code
        self.name = sec_name
        self.lock_group = lock_group
    def run(self):
        history = yfinance.Ticker(self.code).history()
        if not locks[self.lock_group] == None:
            locks[self.lock_group].acquire()
        print(self.code,": ", self.name, self.lock_group)
        print(history)
        print("----------------------------------------------------------------------------")
        if not locks[self.lock_group] == None:
            locks[self.lock_group].release()


thread_list = []
locks = []
datas = []

datas.append(pd.read_csv(r"D:\Dropbox\Workspace\03 Python\12 Finance\Playground\20211116\data_3914_20211116.csv", encoding = "euc_kr"))
datas.append(pd.read_csv(r"D:\Dropbox\Workspace\03 Python\12 Finance\Playground\20211116\data_0618_20211116.csv", encoding = "euc_kr"))

for jj in range(len(datas)):
    locks.append(threading.Lock())
    data = datas[jj]
    locks[jj].acquire()
    for ii in tqdm(range(len(data))):
        security = data.iloc[ii]
        
        if jj == 0:
            sec_code = security["표준코드"]
        elif jj == 1:
            sec_code = f'{security["단축코드"]}.KS'

        sec_name = security["한글종목명"]
        new_thread = YF_grabber(sec_code, sec_name, jj)
        thread_list.append(new_thread)
        new_thread.start()
    locks[jj].release()

for t in thread_list:
    t.join()
print("Done")