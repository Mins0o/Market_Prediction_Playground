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
OUTPUT_LOCK=threading.Lock()

class OhlcvThread(threading.Thread):
    def __init__(self, target_info):
        threading.Thread.__init__(self)
        self.ticker = target_info["ticker"]
        self.fetcherf = target_info["fetcherf"]
        self.result = None
    
    def run(self):
        fetched = pd.DataFrame()
        trial_count=0
        while fetched.empty and trial_count<=100:
            fetched = self.fetcherf(self.ticker)
            trial_count += 1
        if trial_count > 100:
            OUTPUT_LOCK.acquire()
            print(f"\n{self.ticker} failed", end="\r")
            OUTPUT_LOCK.release()
        fetched = fetched.rename(columns=OHLCV_REPLACE_NAMINGS)
        fetched.index.name = "data"
        self.result = fetched

class DataFetcher():
    def __init__(self):
        self._ticker_fetcherf = None
        self._name_fetcherf = None
        self._ohlcv_fetcherf = None
        self._mode = None

        self.ticker_list = None
        self.name_list = None
        self.ohlcv_list = None
        self.compiled_list =None

        self.set_mode("stocks")
        pass

    def set_mode(self, mode = "stocks"):
        if mode.lower() in ["stocks", "stock"]:
            self._set_stock_mode()
        elif mode.lower() in ["etfs", "etf"]:
            self._set_etf_mode()

    def _set_stock_mode(self):
        self._mode = "stocks"
        self._ticker_fetcherf = lambda date: stock.get_market_ticker_list(date, market="ALL")
        self._name_fetcherf = stock.get_market_ticker_name
        self._ohlcv_fetcherf = lambda ticker: stock.get_market_ohlcv("19700101","20301231", ticker)

    def _set_etf_mode(self):
        self._mode = "ETFs"
        self._ticker_fetcherf = stock.get_etf_ticker_list
        self._name_fetcherf = stock.get_etf_ticker_name
        self._ohlcv_fetcherf = lambda ticker: stock.get_etf_ohlcv_by_date("19700101","20301231", ticker)

    def _get_ticker_list(self):
        date_list = [f"{ii}1006" for ii in range(1990,2023,1)]
        ticker_list = []
        for date in date_list:
            tickers = self._ticker_fetcherf(date)
            if tickers:
                ticker_list.append(tickers)
        merge = lambda a, b: pd.merge(pd.DataFrame(a), pd.DataFrame(b), how="outer")
        merged=pd.DataFrame(ticker_list[0])
        for ticker in ticker_list[1:]:
            merged = merge(merged, ticker)
        return merged[0].values.tolist()

    def _get_ticker_names(self, ticker_list):
        return [self._name_fetcherf(ticker) for ticker in ticker_list
                    if len(self._name_fetcherf(ticker)) else ""]

    def _get_ohlcvs_list(self, ticker_list, name_list):
        ohlcv_list=[]
        thread_list = []
        list_size = len(ticker_list)
        get_name = (lambda ii: name_list[ii]) if name_list else (lambda ii: "")
        
        for ticker in ticker_list:
            new_thread = OhlcvThread({
                                    "ticker": ticker,
                                    "fetcherf": self._ohlcv_fetcherf
                                    })
            thread_list.append(new_thread)
        OUTPUT_LOCK.acquire()
        for ii, thread in enumerate(thread_list):
            thread.start()
            print(f" {ii:5d}/{list_size} started", end = "              \r")
        OUTPUT_LOCK.release()
        
        print("\nJoining Threads                  ")
        for ii, thread in enumerate(thread_list):
            thread.join()
            OUTPUT_LOCK.acquire()
            print(f" {ii:5d}/{list_size} {ticker_list[ii]} {thread.ticker} {get_name(ii)} finished", end = "              \r")
            OUTPUT_LOCK.release()
            ohlcv_list.append(thread.result)
        print("Data scraping complete.                        ")
        return ohlcv_list

    def get_all_data(self):
        self.ticker_list = self._get_ticker_list()
        self.name_list = self._get_ticker_names(self.ticker_list)
        self.ohlcv_list = self._get_ohlcvs_list(self.ticker_list, self.name_list)    
        list_size = len(self.ticker_list)
        self.compiled_list = [{"symbol": self.ticker_list[ii],
                                "name": self.name_list[ii],
                                "data": self.ohlcv_list[ii]}
                                for ii in range(list_size) 
                                if not self.ohlcv_list[ii].empty]
        return self.compiled_list
    
    def save_all_data(self):
        all_data = self.get_all_data()
        with open(f"data_pkl/KRX_{self._mode}.pkl", "wb") as file:
            pickle.dump(all_data, file)
        return all_data

if __name__ == "__main__":
    # save_all_data()
    data_fetcher = DataFetcher()

    stocks_data = data_fetcher.save_all_data()

    data_fetcher.set_mode("etf")
    etf_data = data_fetcher.save_all_data()
    pass