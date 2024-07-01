import argparse
from datetime import datetime
import re
import pandas as pd
import pickle
import os.path
import numpy as np

Change = "Change"

def load_data(dir_name="data_pkl", file_name="KRX_stocks.pkl"):
    with open(os.path.join(dir_name,file_name), "rb") as data_file:
        all_data = pickle.load(data_file)
    return all_data

def save_data(all_data, dir_name="data_pkl", file_name="KRX_stocks.pkl"):
    with open(os.path.join(dir_name,file_name), "wb") as save_file:
        pickle.dump(all_data, save_file)

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

def save_combined_data_as_csv(all_data, column_name, dir_name="data_csv", delimitter=",", file_name_suffix=""):
    combined = combine_single_column(all_data, column_name)
    csv_filename = os.path.join(dir_name,f'{column_name}_{file_name_suffix}.csv')
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

def index_data_by_name(data):
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

def search_security_by_name(data, indices, name):
    """
    Search for the data of a security by its name.
    Optimized by using the indices obtained from the `index_data_by_name` function.
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

def merge_data(old_data, new_data, throw_away_old=True):
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
    
    if throw_away_old:
        return overlapping_data + exclusive_data
    for symbol in exclusive_old_symbols:
        entry = search_security(old_data, old_indices, symbol)
        exclusive_data.append(entry)
    
    merged_data = overlapping_data + exclusive_data
    return merged_data

def combine_etf_stock(data_dir="data_pkl"):
    etf_files = [file for file in os.listdir(data_dir) if "ETF" in file and "stock" not in file]
    stock_files = [file for file in os.listdir(data_dir) if "stock" in file and "ETF" not in file]
    etf_file = sorted(etf_files, reverse=True)[0]
    stock_file = sorted(stock_files, reverse=True)[0]

    etf_all_data = load_data(data_dir, etf_file)
    stock_all_data = load_data(data_dir, stock_file)

    etf_all_data = process_etf_data(etf_all_data)
    all_data = merge_data(stock_all_data, etf_all_data, throw_away_old=False)
    return all_data

def diff_entry(old_entry, new_entry):
    old_data_df = old_entry["data"]
    new_data_df = new_entry["data"]
    # Concatenate the dataframes
    combined_df = new_data_df.combine_first(old_data_df)
    # Find rows that only exist in one dataframe or the other
    diff_df = combined_df.loc[~combined_df.index.isin(old_data_df.index) | ~combined_df.index.isin(new_data_df.index)]
    return diff_df

def diff_data(old_data, new_data):
    """
    Find the data that were changed by comparing the symbols of the data.
    Each data is a list of dictionaries with keys "symbol", "name", and "data".
    When getting the diff, specify which row with index (date) is changed.
    Also return the data of, not the symbols only, that have been deleted or added.
    """
    overlapping_symbols, exclusive_old_symbols, exclusive_new_symbols = overlap_deleted_added_data(old_data, new_data)

    old_indices = index_data(old_data)
    new_indices = index_data(new_data)

    changed = []
    deleted = []
    added = []

    for symbol in overlapping_symbols:
        old_entry = search_security(old_data, old_indices, symbol)
        new_entry = search_security(new_data, new_indices, symbol)
        diff_df = diff_entry(old_entry, new_entry)
        if not diff_df.empty:
            changed.append({"symbol": symbol, "name": old_entry["name"], "data": diff_df})

    for symbol in exclusive_old_symbols:
        entry = search_security(old_data, old_indices, symbol)
        deleted.append(entry)

    for symbol in exclusive_new_symbols:
        entry = search_security(new_data, new_indices, symbol)
        added.append(entry)

    return changed, deleted, added

def get_stock_filenames(data_dir="data_pkl", ETF=False):
    """
    Get the filenames of the stock data.
    Args:
        data_dir (str): The directory where the stock data is saved.
    Returns:
        filenames (str): A list of the filenames of the stock data.
    """
    target_phrase = "stock" if not ETF else "etf"
    nontarget_phrase = "etf" if not ETF else "stock"
    stock_files = [filename for filename in os.listdir(data_dir) 
                   if target_phrase in filename.lower() 
                   and nontarget_phrase not in filename.lower()]
    stock_files.sort()
    return stock_files

def get_etf_filenames(data_dir="data_pkl"):
    """
    Get the filenames of the ETF data.
    Args:
        data_dir (str): The directory where the ETF data is saved.
    Returns:
        filenames (str): A list of the filenames of the ETF data.
    """
    etf_files = [filename for filename in os.listdir(data_dir) if "ETF" in filename and "stock" not in filename]
    etf_files.sort()
    return etf_files

def analyze_changes(current_data, merged_data):
    changed, deleted, added = diff_data(current_data, merged_data)
    change_length_analysis = analyze_change_length(changed)
    return changed, deleted, added, change_length_analysis

def analyze_change_length(changed):
    change_length = [len(entry["data"]) for entry in changed]
    change_length_count = {length: change_length.count(length) for length in set(change_length)}
    outlier_analysis = []
    if change_length_count:
        outlier_analysis = identify_outliers(changed, change_length, change_length_count)
    return change_length_count, outlier_analysis

def identify_outliers(changed, change_length, change_length_count):
    most_often_length = max(change_length_count, key=change_length_count.get)
    outlier_indices = [i for i, length in enumerate(change_length) if length != most_often_length]
    outlier_names = [changed[i]["name"] + f"({change_length[i]})" for i in outlier_indices]
    return outlier_names

def interactive_data_merge(data_dir="data_pkl", current_merge_data=None, current_index=None, ETF=False):
    """
    Iteratively merges stock and ETF data files in reverse chronological order from a specified directory.

    This function processes stock and ETF data files by merging them in reverse order, starting with the two most recent files. 
    After each merge, it provides a concise overview of the differences compared to the previous iteration, including changes, deletions, and additions. 
    It specifically highlights any outliers in the change lengths of the data, suggesting potential inconsistencies or anomalies in the data changes. 
    These outliers are flagged for manual review. The function aims to give a clear picture of how the merged data evolves with each file processed.

    Parameters:
    - data_dir (str): The directory where the stock and ETF data files are located. Defaults to "data_pkl".

    Outputs:
    - Console output that includes:
        - The name of the current file being processed.
        - The length of the data before and after merging.
        - Detailed information on the differences after each merge, including:
            - The number of changed, deleted, and added data entries.
            - A count of the lengths of changed data entries to identify outliers.
            - The names of securities with outlier changes, deletions, and additions.

    The function is designed for interactive use, providing insights into the data merging process and highlighting areas that may require further investigation.
    """
    files = get_stock_filenames(data_dir, ETF)
    data_merged = load_data(data_dir, files[-1])
    if current_merge_data is not None and current_index is not None:
        data_merged = current_merge_data
        files = files[:-current_index+1]
    print("new", files[-1], len(data_merged))
    ii=-1
    for ii in range(-2, -len(files)-1, -1):
        file_ = files[ii]
        data_ = load_data(data_dir, file_)
        print("old", file_, len(data_))
        previous_data_merged = data_merged
        data_merged = merge_data(data_, data_merged, throw_away_old=True)

        changed, deleted, added = diff_data(data_, data_merged)
        change_length_count, outlier_names = analyze_change_length(changed)

        print("Changed:", len(changed), change_length_count, outlier_names)
        print("deleted", len(deleted), [entry["name"] for entry in deleted])
        print("added", len(added), [entry["name"] for entry in added])

        # interactive part
        print("Review the changes.")
        print("1. Keep the merge")
        print("2. Revert the current merge")
        print("3. Stop here (cancels current merge)")
        print("4. Save and stop here")
        print("5. Abort everything (Don't save anything and quit)")
        print("""If you want to review in detail, 
              stop here by entering 3 and run the function with the current data.
              use the return of the function as the current_merge_data and current_index.""")
        choice = input("Enter your choice: ")
        if choice == "1":
            pass
        elif choice == "2":
            data_merged = previous_data_merged
        elif choice == "3":
            return previous_data_merged, ii
        elif choice == "4":
            return data_merged, ii-1
        elif choice == "5":
            return None, None
    return data_merged, ii

def investigate_with_result(current_merge, file_index, stock_name, dir_name="data_pkl", ETF=False):
    files = get_stock_filenames(dir_name, ETF)
    old_data=load_data(dir_name, files[file_index])
    new_data=load_data(dir_name, files[-1])
    old_indices = index_data_by_name(old_data)
    new_indices = index_data_by_name(new_data)

    print("Old data", files[file_index])
    old_entry = search_security_by_name(old_data, old_indices, stock_name)
    if old_entry:
        print(old_entry["data"])

    print("New data", files[-1])
    new_entry = search_security_by_name(new_data, new_indices, stock_name)
    if new_entry:
        print(new_entry["data"])
    else:
        print(f"The name {stock_name} is not found in the new data.")

    print("Merged data")
    merged_entry = search_security_by_name(current_merge, index_data_by_name(current_merge), stock_name)
    if merged_entry:
        print(merged_entry["data"])
    else:
        print(f"The name {stock_name} is not found in the merged data.")

    print("Diff data")
    if new_entry and old_entry:
        diff_df = diff_entry(old_entry, new_entry)
        if diff_df is not None:
            print(diff_df)
    else:
        print("Cannot find the data to compare.")

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Merge stock and ETF data files interactively.")
    parser.add_argument("--auto", action="store_true", help="Automatically merge stock and ETF data files.")
    if parser.parse_args().auto:
        stocks_merged, index = interactive_data_merge("../data_pkl")
        etf_merged, index = interactive_data_merge("../data_pkl", ETF=True)
        etf_merged = process_etf_data(etf_merged)
        print("len stocks, len etfs", len(stocks_merged), len(etf_merged))
        combined = merge_data(stocks_merged, etf_merged, throw_away_old=False)
        print("combined_length", len(combined))
        combined.sort(key=lambda x: x["name"])
        date_string = re.sub(r'[\W-]','_',datetime.now().__str__()[:19])
        save_data(etf_merged, "../data_pkl", f"KRX_etfs_{date_string}.pkl")
        save_data(stocks_merged, "../data_pkl", f"KRX_stocks_{date_string}.pkl")
        save_data(combined, "../data_pkl", 
                  f"KRX_stocks_etf_combined_{date_string}.pkl")
        save_combined_data_as_csv(combined, "Change", "../data_csv", delimitter="\t", file_name_suffix=date_string)
