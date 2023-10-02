import pandas as pd
import pickle


def load_data():
    with open("data_pkl/KRX_data.pkl", "rb") as data_file:
        all_data = pickle.load(data_file)
    return all_data

def combine_single_column(all_data, column_name):
    columns_collected = [df["data"][column_name] for df in all_data]
    for ii, column in enumerate(columns_collected):
        column.name=all_data[ii]["name"]
    combined = pd.concat(columns_collected, axis=1)
    return combined

def save_combined_data(all_data, column_name):
    combined = combine_single_column(all_data, column_name)
    with open(f"data_pkl/{column_name}.pkl", "wb") as save_file:
        pickle.dump(combined, save_file)
