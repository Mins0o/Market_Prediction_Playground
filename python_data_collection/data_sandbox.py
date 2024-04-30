import pandas as pd
import pickle
import os.path
import numpy as np


def load_data(dir_name="data_pkl", file_name="KRX_stocks.pkl"):
    with open(os.path.join(dir_name,file_name), "rb") as data_file:
        all_data = pickle.load(data_file)
    return all_data

def combine_single_column(all_data, column_name):
    columns_collected = [df["data"][column_name] for df in all_data]
    for ii, column in enumerate(columns_collected):
        column.name=all_data[ii]["name"]
    combined = pd.concat(columns_collected, axis=1)
    return combined

def save_combined_data(all_data, column_name, dir_name="data_pkl"):
    combined = combine_single_column(all_data, column_name)
    with open(os.path.join(dir_name,column_name)+".pkl", "wb") as save_file:
        pickle.dump(combined, save_file)

def save_combined_data_as_csv(all_data, column_name, dir_name="data_csv", delimitter=","):
    combined = combine_single_column(all_data, column_name)
    csv_filename = os.path.join(dir_name,f'{column_name}.csv')
    combined.to_csv(csv_filename, sep = delimitter)

def process_etf_data(etf_all_data):
    for entry in etf_all_data:
        change=[]
        prev=1
        for close in entry["data"]["Close"]:
            change.append(((close/prev)-1)*100)
            prev = close
        change[0] = np.NaN
        entry["data"]["Change"] = change
    return etf_all_data