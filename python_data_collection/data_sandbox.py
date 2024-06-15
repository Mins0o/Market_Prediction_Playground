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

def find_added_and_deleted_data(old_data, new_data):
    """
    Find the data that were added and deleted by comparing the names of the data.
    Each data is a list of dictionaries with keys "symbol", "name", and "data".
    """
    old_names = set([entry["name"] for entry in old_data])
    new_names = set([entry["name"] for entry in new_data])
    added = new_names - old_names
    deleted = old_names - new_names
    return added, deleted

def index_data(data):
    """
    Index the data by the names of the securities.
    The return indices will be used in the `search_security` function.
    Args:
        data (list): A list of dictionaries containing the symbol, name, and data for each security.
    Returns:
        indices (dict): A dictionary with the names of the securities as keys and the corresponding index as values.
    """
    indices = {}
    for i, entry in enumerate(data):
        indices[entry["name"]] = i
    return indices

def search_security(data, indices, name):
    """
    Search for the data of a security by its name.
    Optimized by using the indices obtained from the `index_data` function.
    Args:
        data (list): A list of dictionaries containing the symbol, name, and data for each security.
        indices (dict): A dictionary with the names of the securities as keys and the corresponding index as values.
        name (str): The name of the security to search for.
    Returns:
        security_data (dict): A dictionary containing the symbol, name, and data for the specified security.
    """
    index = indices.get(name)
    if index is not None:
        return data[index]
    else:
        return None
