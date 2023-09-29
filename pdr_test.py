import pandas_datareader as pdr
import pandas as pd
from tqdm import tqdm
import threading
import matplotlib.pyplot as plt
import pickle

CODE_COL = "종목코드"
NAME_COL = "종목명"

class NvFetcher(threading.Thread):
    def __init__(self, sec_data):
        threading.Thread.__init__(self)
        self.data = sec_data
        self.result=None
    def run(self):
        try:
            code = f'{int(self.data[CODE_COL]):06d}'
        except ValueError:
            code = str(self.data[CODE_COL])
        name = self.data[NAME_COL]
        
        reader = search(code)
        try:
            self.result = reader.read()
            error = "ok"
        except :
            self.result = "Error"
            error = "Error"
        reader.close()
        output_lock.acquire()
        # print(code, name,'\n', result, error)
        # print("----------------------------------------------------------------------------")
        output_lock.release()

data = pd.read_csv(r"D:\Workspace\03 Python\12 Finance\Playground\20230928\data_1921_20230928.csv", encoding = "utf8")[[CODE_COL, NAME_COL]]
# data = pd.concat([data,pd.read_csv(r"D:\Workspace\03 Python\12 Finance\Playground\20211116\data_0618_20211116.csv", encoding = "utf8")[["단축코드", "한글종목약명"]]])
data.set_index(pd.Series(range(len(data))))

output_lock = threading.Lock()
search = lambda x: pdr.naver.NaverDailyReader(x,start="19700101")

threads = []
for _, ii in tqdm(data.iterrows(), total = len(data)):
    new_thread = NvFetcher(ii)
    threads.append(new_thread)
    new_thread.start()

for t in tqdm(threads):
    t.join()
print("Done")

output_data=[(thread.data, thread.result) for thread in threads]
with open("test.pkl", "wb") as f:
    pickle.dump(output_data, f)

# 삼성전자 1030

my_data = threads[1030].result
my_plot = plt.figure()
my_subplots = my_plot.subplots(2,2)
my_subplots[0,0].plot(my_data.index, my_data["Close"].astype(int))
