from pykrx import stock
import pandas as pd
import pickle
from tqdm import tqdm

OHLCV_REPLACE_NAMINGS = {"날짜": "date", 
                         "시가": "Open", 
                         "고가": "High", 
                         "저가": "Low", 
                         "종가": "Close", 
                         "거래량": "Volume", 
                         "등락률": "Change"}

def get_stock_tickers():
    date_list = [f"{ii}0101" for ii in range(2000,2023,1)]
    ticker_list = [stock.get_market_ticker_list(date, market="ALL") for date in date_list]
    merge = lambda a, b: pd.merge(pd.DataFrame(a), pd.DataFrame(b), how="outer")
    merged=pd.DataFrame(ticker_list[0])
    for ticker in ticker_list:
        merged = merge(merged, ticker)
    return merged[0].values.tolist()

def get_etf_tickers():
    date_list = [f"{ii}0101" for ii in range(2005,2023,1)]
    ticker_list = [stock.get_etf_ticker_list(date) for date in date_list]
    merge = lambda a, b: pd.merge(pd.DataFrame(a), pd.DataFrame(b), how="outer")
    merged=pd.DataFrame(ticker_list[0])
    for ticker in ticker_list:
        merged = merge(merged, ticker)
    return merged[0].values.tolist()

def get_stock_ticker_names(ticker_list):
    return [stock.get_market_ticker_name(ticker) for ticker in ticker_list]

def get_etf_ticker_names(ticker_list):
    return [stock.get_etf_ticker_name(ticker) for ticker in ticker_list]

def fetch_stock_ohlcvs(ticker):
    fetched = stock.get_market_ohlcv("19700101","20301231", ticker)
    fetched = fetched.rename(columns=OHLCV_REPLACE_NAMINGS)
    fetched.index.name="data"
    return fetched

def fetch_etf_ohlcvs(ticker):
    fetched = stock.get_etf_ohlcv_by_date("19700101","20301231", ticker)
    fetched = fetched.rename(columns=OHLCV_REPLACE_NAMINGS)
    fetched.index.name="data"
    return fetched

def get_stock_ticker_list_ohlcvs(ticker_list, name_list=None):
    all_data=[]
    list_size = len(ticker_list)
    if name_list is not None:
        for ii, ticker in enumerate(ticker_list):
            print(f" {ii:5d}/{list_size} {name_list[ii]}", end = "              \r")
            all_data.append(fetch_stock_ohlcvs(ticker))
    else:
        all_data = [fetch_stock_ohlcvs(ticker) for ticker in tqdm(ticker_list)]
    print("Data scraping complete.                        ")
    return all_data

def get_etf_ticker_list_ohlcvs(ticker_list, name_list=None):
    all_data=[]
    list_size = len(ticker_list)
    if name_list is not None:
        for ii, ticker in enumerate(ticker_list):
            print(f" {ii:5d}/{list_size} {name_list[ii]}", end = "              \r")
            all_data.append(fetch_etf_ohlcvs(ticker))
    else:
        all_data = [fetch_etf_ohlcvs(ticker) for ticker in tqdm(ticker_list)]
    print("Data scraping complete.                        ")
    return all_data

def get_all_data():
    ticker_list = get_stock_tickers()
    name_list = get_stock_ticker_names(ticker_list)
    market_records = get_stock_ticker_list_ohlcvs(ticker_list, name_list)
    
    list_size = len(ticker_list)
    all_data = [{"symbol": ticker_list[ii],"name": name_list[ii],"data": market_records[ii]}
                for ii in range(list_size) if not market_records[ii].empty]
    return all_data

def save_all_data():
    all_data = get_all_data()
    with open("data_pkl/KRX_data.pkl","wb") as file:
        pickle.dump(all_data, file)

if __name__ == "__main__":
    # save_all_data()
    pass