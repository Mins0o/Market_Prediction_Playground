import numpy as np
import pandas as pd
import matplotlib.pyplot as plt

with open("../data_csv/KRX_stocks_change_2024-04-20.tsv", "r") as f:
    test = pd.read_csv(f, sep='\t')


ss = test["삼성전자"]
ss_nn = ss.fillna(0)

ss_hist = {}
for bin_count in [10,11,20,21,30,31,40,41,50,51]:
    ss_hist[bin_count]=np.histogram(ss_nn, bins=bin_count, range=(-30,30))
    plt.hist(ss, bins=bin_count,range=(-30,30))
no_na_ss = ss.dropna()
print(no_na_ss.values.mean())
exponents = np.exp(-(no_na_ss.size - np.arange(no_na_ss.size) - 1)/500)
exp_sum = exponents.sum()
test_weighted = no_na_ss.values * exponents

discounted_avg = test_weighted.sum()/exp_sum

plt.savefig("varying_bin_count.png")
