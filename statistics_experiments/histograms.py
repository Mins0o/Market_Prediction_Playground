import numpy as np
import pandas as pd
import matplotlib.pyplot as plt

with open("../data_csv/KRX_stocks_change_2024-04-20.tsv", "r") as f:
    test = pd.read_csv(f, sep='\t')


ss = test["삼성전자"]
ss_nn = ss.fillna(0)
no_na_ss = ss.dropna()

ss_hist = {}
for bin_count in [10,11,20,21,30,31,40,41,50,51]:
    ss_hist[bin_count]=np.histogram(no_na_ss, bins=bin_count, range=(-30,30))
    plt.hist(ss, bins=bin_count,range=(-30,30))

plt.savefig("varying_bin_count.png")
plt.clf()

exponents = np.exp(-(no_na_ss.size - np.arange(no_na_ss.size) - 1)/500)
exp_sum = exponents.sum()
test_weighted = no_na_ss.values * exponents

discounted_avg = test_weighted.sum()/exp_sum
is_this_okay = test_weighted.sum()/len(test_weighted)
arithmetic_avg = no_na_ss.values.mean()

print(f"discounted_avg: {discounted_avg}")
print(f"is_this_okay: {is_this_okay}")
print(f"arithmetic_avg: {arithmetic_avg}")

plt.subplot(2,1,1)
pasts=no_na_ss.values[:-len(no_na_ss.values)//8]
plt.hist(pasts, bins=101, range=(-5,5))
plt.subplot(2,1,2)
recents=no_na_ss.values[-len(no_na_ss.values)//8:]
plt.hist(recents, bins=101, range=(-5,5))
plt.savefig("recent_vs_past")

print(f"pasts_avg: {pasts.mean()}")
print(f"recents_avg: {recents.mean()}")