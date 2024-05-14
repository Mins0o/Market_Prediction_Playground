import numpy
import pandas as pd
import matplotlib.pyplot as plt

with open("../data_csv/KRX_stocks_change_2024-04-20.tsv", "r") as f:
    test = pd.read_csv(f, sep='\t')


ss = test["삼성전자"]
ss_hist = ss.hist(bins=10)

plt.hist(ss, bins=10)
plt.savefig("hello.png")