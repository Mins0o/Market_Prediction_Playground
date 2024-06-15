from pykrx import stock
import pandas as pd
import pickle
from tqdm import tqdm
import threading
from datetime import datetime
import time

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
        self.security = target_info["security"]
        self.fetcherf = target_info["fetcherf"]
        self.result = None
    
    def run(self):
        fetched = pd.DataFrame()
        trial_count=0
        while fetched.empty and trial_count<=100:
            fetched = self.fetcherf(self.security)
            trial_count += 1
            time.sleep(60)
        if trial_count > 100:
            OUTPUT_LOCK.acquire()
            print(f"\n{self.security} failed")
            OUTPUT_LOCK.release()
        fetched = fetched.rename(columns=OHLCV_REPLACE_NAMINGS)
        fetched.index.name = "data"
        self.result = fetched

class DataFetcher():
    def __init__(self):
        self._security_fetcherf = None
        self._name_fetcherf = None
        self._ohlcv_fetcherf = None
        self._mode = None

        self.security_list = None
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
        self._security_fetcherf = lambda date: stock.get_market_ticker_list(date, market="ALL")
        self._name_fetcherf = stock.get_market_ticker_name
        self._ohlcv_fetcherf = lambda security: stock.get_market_ohlcv("19700101","20301231", str(security))

    def _set_etf_mode(self):
        self._mode = "ETFs"
        self._security_fetcherf = stock.get_etf_ticker_list
        self._name_fetcherf = stock.get_etf_ticker_name
        self._ohlcv_fetcherf = lambda security: stock.get_etf_ohlcv_by_date("19700101","20301231", str(security))

    def _get_security_list(self, year_range = 0):
        """
        Gets a list of securities available within the specified year range.

        Args:
            year_range (int): The number of years to go back from the current year to fetch securities.
                              For a value of 0, it fetches currently available stocks. Other values
                              indicate how many years back from the current year to include securities
                              that are no longer available.

        Returns:
            list: A list of securities merged from all years within the specified range.
        """
        
        today = datetime.today()
        date_list = [f"{year}{today.month:02d}{today.day:02d}" for year in range(today.year, today.year-1-year_range, -1)]
        security_list = []
        for date in date_list:
            securities = self._security_fetcherf(date)
            if securities:
                security_list.append(securities)
        merge = lambda a, b: pd.merge(pd.DataFrame(a), pd.DataFrame(b), how="outer")
        merged=pd.DataFrame(security_list[0])
        for security in security_list[1:]:
            merged = merge(merged, security)
        return merged[0].values.tolist()

    def _get_security_names(self, security_list):
        name_list = []
        for security in security_list:
            name = self._name_fetcherf(security)
            if not len(name):
                name = f"{security} No Name"
            name_list.append(name)
        return name_list

    def _get_ohlcvs_list_non_threaded(self, security_list, name_list):
        ohlcv_list=[]
        list_size = len(security_list)
        get_name = (lambda ii: name_list[ii]) if name_list else (lambda ii: "")
        TRIAL_CNT=0
        for ii, security in enumerate(security_list):
            fetched = pd.DataFrame()
            trial_count=0
            fetched = self._ohlcv_fetcherf(security)
            while fetched.empty and trial_count<=TRIAL_CNT:
                time.sleep(10)
                try:
                    fetched = self._ohlcv_fetcherf(security)
                except Exception as e:
                    print(e)
                    fetched = pd.DataFrame()
                trial_count += 1
            if trial_count > TRIAL_CNT:
                print(f"\n{security} failed")
            fetched = self._ohlcv_fetcherf(security)
            fetched = fetched.rename(columns=OHLCV_REPLACE_NAMINGS)
            fetched.index.name = "data"
            print(f" {ii+1:5d}/{list_size} {security_list[ii]} {get_name(ii)} finished", end = "                       \r")
            ohlcv_list.append(fetched)
        return ohlcv_list

    def _get_ohlcvs_list_threaded(self, security_list, name_list):
        ohlcv_list=[]
        thread_list = []
        list_size = len(security_list)
        get_name = (lambda ii: name_list[ii]) if name_list else (lambda ii: "")
        for security in security_list:
            new_thread = OhlcvThread({
                                    "security": security,
                                    "fetcherf": self._ohlcv_fetcherf
                                    })
            thread_list.append(new_thread)
        
        OUTPUT_LOCK.acquire()
        for ii, thread in enumerate(thread_list):
            thread.start()
            print(f" {ii+1:5d}/{list_size} started", end = "              \r")
        OUTPUT_LOCK.release()
        
        print("\nJoining Threads                  ")
        for ii, thread in enumerate(thread_list):
            thread.join()
            OUTPUT_LOCK.acquire()
            print(f" {ii+1:5d}/{list_size} {security_list[ii]} {thread.security} {get_name(ii)} finished", end = "                       \r")
            OUTPUT_LOCK.release()
            ohlcv_list.append(thread.result)
        print("Data scraping complete.                        ")
        return ohlcv_list

    def _get_ohlcvs_list(self, security_list, name_list, thread_mode=False):
        """
        Retrieves OHLCV (Open, High, Low, Close, Volume) data for a list of securities.

        Args:
            security_list (list): A list of securities for which OHLCV data needs to be retrieved.
            name_list (list): A list of names corresponding to the securities in `security_list`.
            thread_mode (bool, optional): Specifies whether to use threaded mode for data retrieval. 
                                          Defaults to False.

        Returns:
            list: A list of OHLCV data for the given securities.

        """
        if thread_mode:
            return self._get_ohlcvs_list_threaded(security_list, name_list)
        else:
            return self._get_ohlcvs_list_non_threaded(security_list, name_list)

    def get_all_data(self):
        """
        Retrieves all data for the specified mode.

        Returns:
            list: A list of dictionaries containing the symbol, name, and data for each security.
        """
        print(f"Getting {self._mode}")
        self.security_list = self._get_security_list(0)
        self.name_list = self._get_security_names(self.security_list)
        self.ohlcv_list = self._get_ohlcvs_list(self.security_list, self.name_list)    
        list_size = len(self.security_list)
        self.compiled_list = [{"symbol": self.security_list[ii],
                                "name": self.name_list[ii],
                                "data": self.ohlcv_list[ii]}
                                for ii in range(list_size) 
                                if not self.ohlcv_list[ii].empty]
        return self.compiled_list
    
    def save_all_data(self):
        """Saves all the data obtained from get_all_data() method to a pickle file.

        Returns:
            list: The list of all data obtained.
        """
        all_data = self.get_all_data()
        with open(f"../data_pkl/KRX_{self._mode}_{datetime.now().__str__()[:10]}.pkl", "wb") as file:
            pickle.dump(all_data, file)
        return all_data

if __name__ == "__main__":
    data_fetcher = DataFetcher()

    stocks_data = data_fetcher.save_all_data()

    data_fetcher.set_mode("etf")
    etf_data = data_fetcher.save_all_data()
    pass
