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

def overlap_deleted_added_data(old_data, new_data):
    """
    Find the data that were added and deleted by comparing the symbols of the data.
    Each data is a list of dictionaries with keys "symbol", "name", and "data".
    """
    old_symbols = set([entry["symbol"] for entry in old_data])
    new_symbols = set([entry["symbol"] for entry in new_data])
    overlap = old_symbols & new_symbols
    added = new_symbols - old_symbols
    deleted = old_symbols - new_symbols
    return overlap, deleted, added

def index_data(data):
    """
    Index the data by the symbols of the securities.
    The return indices will be used in the `search_security` function.
    Args:
        data (list): A list of dictionaries containing the symbol, name, and data for each security.
    Returns:
        indices (dict): A dictionary with the symbols of the securities as keys and the corresponding index as values.
    """
    indices = {}
    for i, entry in enumerate(data):
        indices[entry["symbol"]] = i
    return indices

def search_security(data, indices, symbol):
    """
    Search for the data of a security by its symbol.
    Optimized by using the indices obtained from the `index_data` function.
    Args:
        data (list): A list of dictionaries containing the symbol, name, and data for each security.
        indices (dict): A dictionary with the symbols of the securities as keys and the corresponding index as values.
        symbol (str): The symbol of the security to search for.
    Returns:
        security_data (dict): A dictionary containing the symbol, name, and data for the specified security.
    """
    index = indices.get(symbol)
    if index is not None:
        return data[index]
    else:
        return None

def merge_data(old_data, new_data):
    """
    Merge two sets of data.
    
    Each data is a list of dictionaries with keys "symbol", "name", and "data".
    Majority of the entries overlap, but the newer data have more rows in the data (Pandas.DataFrame).
    First, compare the entries in the data and separate the exclusive ones with overlapping ones.
    With the overlapping entries in the list, merge the value (Pandas.Dataframe) of "data" key of the two entries.
    The overlapping data values (Pandas.Dataframe) from old and new share majority of the rows, but the newer one have more on the bottom.
    The data values are indexed by dates.
    Compile the updated overlapping entries and the exclusive entries from the new data.
    """
    overlapping_symbols, exclusive_old_symbols, exclusive_new_symbols = overlap_deleted_added_data(old_data, new_data)

    old_indices = index_data(old_data)
    new_indices = index_data(new_data)
    
    overlapping_data = []
    for symbol in overlapping_symbols:
        old_entry = search_security(old_data, old_indices, symbol)
        new_entry = search_security(new_data, new_indices, symbol)
        old_df = old_entry["data"]
        new_df = new_entry["data"]
        merged_df = new_df.combine_first(old_df)
        overlapping_data.append({"symbol": symbol, "name": old_entry["name"], "data": merged_df})
    
    exclusive_data = []
    for symbol in exclusive_new_symbols:
        entry = search_security(new_data, new_indices, symbol)
        exclusive_data.append(entry)
    
    for symbol in exclusive_old_symbols:
        entry = search_security(old_data, old_indices, symbol)
        exclusive_data.append(entry)
    
    merged_data = overlapping_data + exclusive_data
    return merged_data