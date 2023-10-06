from pykrx import stock
import pandas as pd
import pickle
from tqdm import tqdm
import threading

OHLCV_REPLACE_NAMINGS = {"날짜": "date", 
                         "시가": "Open", 
                         "고가": "High", 
                         "저가": "Low", 
                         "종가": "Close", 
                         "거래량": "Volume", 
                         "등락률": "Change"}

class OhlcvThread(threading.Thread):
    def __init__(self, target_info):
        threading.Thread.__init__(self)
        self.ticker = target_info["ticker"]
        self.fetcher = target_info["fetcher"]
        self.result = None
    
    def run(self):
        fetched = self.fetcher(self.ticker)
        fetched = fetched.rename(columns=OHLCV_REPLACE_NAMINGS)
        fetched.index.name = "data"
        self.result = fetched

class DataFetcher():
    def __init__(self):
        self._ticker_fetcher = None
        self._name_fetcher = None
        self._ohlcv_fetcher = None
        self._mode = None

        self.set_mode("stocks")
        pass

    def set_mode(self, mode = "stocks"):
        if mode.lower() in ["stocks", "stock"]:
            self._set_stock_mode()
        elif mode.lower() in ["etfs", "etf"]:
            self._set_etf_mode()

    def _set_stock_mode(self):
        self._mode = "stocks"
        self._ticker_fetcher = lambda date: stock.get_market_ticker_list(date, market="ALL")
        self._name_fetcher = stock.get_market_ticker_name
        self._ohlcv_fetcher = lambda ticker: stock_market_ohlcv("19700101","20301231", ticker)

    def _set_etf_mode(self):
        self._mode = "ETFs"
        self._ticker_fetcher = stock.get_etf_ticker_list
        self._name_fetcher = stock.get_etf_ticker_name
        self._ohlcv_fetcher = lambda ticker: stock_etf_ohlcv_by_date("19700101","20301231", ticker)

    def _get_ticker_list(self):
        date_list = [f"{ii}0101" for ii in range(2000,2023,1)]
        ticker_list = []
        for date in date_list:
            tickers = self._ticker_fetcher(date)
            if tickers:
                ticker_list.append(tickers)
        merge = lambda a, b: pd.merge(pd.DataFrame(a), pd.DataFrame(b), how="outer")
        merged=pd.DataFrame(ticker_list[0])
        for ticker in ticker_list[1:]:
            merged = merge(merged, ticker)
        return merged[0].values.tolist()

    def _get_ticker_names(self, ticker_list):
        return [self._name_fetcher(ticker) for ticker in ticker_list]

    def _get_ohlcvs_list(self, ticker_list, name_list):
        all_data=[]
        thread_list = []
        list_size = len(ticker_list)
        get_name = (lambda ii: name_list[ii]) if name_list else (lambda ii: "")
        
        for ii, ticker in enumerate(ticker_list):
            new_thread = OhlcvThread({
                                    "ticker": ticker,
                                    "fetcher": self._ohlcv_fetcher
                                    })
            thread_list.append(new_thread)
            new_thread.start()
        
        for ii, thread in enumerate(thread_list):
            print(f" {ii:5d}/{list_size} {get_name(ii)}", end = "              \r")
            thread_join()
            all_data.append(thread.result)
        print("Data scraping complete.                        ")
        return all_data

    def get_all_data():
        ticker_list = self._get_ticker_list()
        name_list = self._get_ticker_names(ticker_list)
        market_records = self._get_ohlcvs_list(ticker_list, name_list)
        print(market_records[0], market_records[500])

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