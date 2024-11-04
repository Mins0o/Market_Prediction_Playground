#!/usr/bin/bash
read -p "Enter start date (YYYYMMDD): " start_date && \
. ../pye_fin/bin/activate && \
python data_scraping_sandbox.py --date $start_date && \
ls ../data_pkl -go && \
python data_sandbox.py --auto && \
ls ../data_pkl -got
ls ../data_csv -got
deactivate
