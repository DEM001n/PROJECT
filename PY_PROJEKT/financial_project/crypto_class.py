import csv
import requests
from datetime import datetime, timedelta

import os
import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
from scipy.stats import boxcox
from statsmodels.tsa.statespace.sarimax import SARIMAX


class CryptoDataFetcher:

    def __init__(self, name, step=None, time_request=None, data_time=None):
        self.name = name.upper()

        if step and time_request:
            self.step = step
            self.time_request = time_request
        elif data_time:
            self.step, self.time_request = self._make_request(data_time)
        else:
            raise ValueError("Either step and time_request, or data_time must be provided.")

    @staticmethod
    def _make_request(data_time):
        now = datetime.now()

        if data_time == 'w':
            step = '1h'
            time_request = now - timedelta(days=7)
        elif data_time == 'm':
            step = '3h'
            time_request = now - timedelta(days=30)
        elif data_time == 'y':
            step = '1d'
            time_request = now - timedelta(days=365)
        else:
            raise ValueError("Wrong period! Enter 'w'-week, 'm'-month or 'y'-year.")

        return step, int(time_request.timestamp())

    @staticmethod
    def _request_validation(response):
        if response.status_code == 200:
            return response
        else:
            raise requests.exceptions.HTTPError(f"Invalid response: {response.status_code}")

    def _get_id(self):
        try:
            response = requests.get(f"https://api.coinmarketcap.com/data-api/v1/cryptocurrency/map?symbol={self.name}",
                                    timeout=120)
            data = CryptoDataFetcher._request_validation(response).json()
            eth_id = data['data'][0]['id']
            return eth_id
        except requests.exceptions.RequestException as e:
            raise ConnectionError(f"Error retrieving ID for {self.name}: {e}")

    def fetch_crypto_data(self):
        try:
            response = requests.get(
                f'https://api.coinmarketcap.com/data-api/v3.1/cryptocurrency/historical?id={self._get_id()}'
                f'&timeStart={self.time_request}&interval={self.step}&convertId=2781',
                timeout=120
            )
            data = CryptoDataFetcher._request_validation(response).json()
            return data['data']
        except requests.exceptions.RequestException as e:
            raise ConnectionError(f"Request error: {e}")
        except Exception as e:
            raise RuntimeError(f"An unexpected error occurred: {e}")

class CryptoDataSaver:

    def __init__(self, output_file, crypto_data):
        self.output_file = output_file
        self.crypto_data = crypto_data

    def save_to_csv(self):
        crypto_name = self.crypto_data['name']
        print(f"Cryptocurrency: {crypto_name}")

        with open(self.output_file, mode='w', newline='') as csv_file:
            csv_writer = csv.writer(csv_file, delimiter=';')
            csv_writer.writerow(["name", "open", "high", "low", "close", "volume", "marketCap", "timestamp"])
            for quote in self.crypto_data['quotes']:
                timestamp = quote['timeOpen']
                formatted_timestamp = datetime.fromisoformat(timestamp[:-1]).strftime('%Y-%m-%dT%H:%M:%S.%fZ')
                csv_writer.writerow([
                    crypto_name,
                    quote['quote']['open'],
                    quote['quote']['high'],
                    quote['quote']['low'],
                    quote['quote']['close'],
                    quote['quote']['volume'],
                    quote['quote']['marketCap'],
                    formatted_timestamp
                ])
            print(f"Data saved to {self.output_file}")

class CryptoDataManager(CryptoDataFetcher, CryptoDataSaver):

    def __init__(self, output_file, name, data_time):
        CryptoDataFetcher.__init__(self, name, data_time=data_time)
        self.output_file = output_file

    def get_and_save_crypto_data(self):
        try:
            crypto_data = self.fetch_crypto_data()
            saver = CryptoDataSaver(self.output_file, crypto_data)
            saver.save_to_csv()
        except Exception as e:
            raise RuntimeError(f"An error occurred while fetching or saving crypto data: {e}")


class CryptoDataAnalysis:

    def __init__(self, data=None, filename=None):
        if filename is not None:
            self._data = self.load_data(filename)
        elif isinstance(data, pd.DataFrame):
            self._data = data
        else:
            raise ValueError("You must provide either a DataFrame or a filename.")

        self._data_predictions = None
        self._sar = None
        self._crypto_name = self.data['name'].iloc[0]

    @staticmethod
    def load_data(filename):
        if not os.path.isfile(filename):
            raise FileNotFoundError(f"Error: file not found at path {filename}.")
        try:
            data = pd.read_csv(filename, sep=';', quotechar='"')
            data['timestamp'] = pd.to_datetime(data['timestamp'], errors='coerce')
            data = data.dropna(subset=['timestamp'])
            data = data.set_index('timestamp')

            numeric_columns = ['volume', 'marketCap', 'close', 'open', 'high', 'low']
            data[numeric_columns] = data[numeric_columns].apply(pd.to_numeric, errors='coerce')

            data['volume_ma'] = data['volume'].rolling(window=3).mean()
            data['marketCap_ma'] = data['marketCap'].rolling(window=3).mean()
            data = data.dropna(subset=['volume_ma', 'marketCap_ma'])
            return data
        except Exception as e:
            raise IOError(f"An error occurred: {e}")

    def calculate_sar(self, step=0.02, max_step=0.2): #SAR_i = SAR_i−1 +AF×(EP−SAR_i−1)
        data = self._data
        sar = np.zeros(len(data))
        trend = 1
        ep = data['high'].iloc[0]
        sar[0] = data['low'].iloc[0]
        af = step

        for i in range(1, len(data)):
            sar[i] = sar[i - 1] + step * (ep - sar[i - 1])

            if trend == 1:
                if data['low'].iloc[i] < sar[i]:
                    trend = -1
                    sar[i] = ep
                    ep = data['low'].iloc[i]
                    af = step
            else:
                if data['high'].iloc[i] > sar[i]:
                    trend = 1
                    sar[i] = ep
                    ep = data['high'].iloc[i]
                    af = step

            if trend == 1:
                ep = max(ep, data['high'].iloc[i])
                af = min(af + step, max_step)
            else:
                ep = min(ep, data['low'].iloc[i])
                af = min(af + step, max_step)

        self._sar = sar

    def get_calculate_stability_coefficient_and_info(self):
        data = self._data
        stability_coefficient = (data['volume'] / data['marketCap']) * 100

        average_coefficient = stability_coefficient.mean()
        average_volume = data['volume'].mean()
        average_market_cap = data['marketCap'].mean()

        return average_coefficient, average_volume, average_market_cap

    def _determine_time_step(self):
        data = self._data
        time_range = data.index[-1] - data.index[0]

        if time_range <= pd.Timedelta(days=10):
            season = 7
            predict_end_delta = pd.Timedelta(hours=6)
        elif time_range <= pd.Timedelta(days=45):
            season = 4
            predict_end_delta = pd.Timedelta(days=1)
        elif time_range <= pd.Timedelta(days=400):
            season = 12
            predict_end_delta = pd.Timedelta(weeks=1)
        else:
            season = 5
            predict_end_delta = pd.Timedelta(days=30)

        return season, predict_end_delta

    @staticmethod
    def _inv_boxcox(y, lam):
        if lam == 0:
            return np.exp(y)
        else:
            return np.exp(np.log(lam * y + 1) / lam)

    @staticmethod
    def _unique_count(arr):
        return len(np.unique(arr))

    @staticmethod
    def get_sarimax_predict(prices, steps, season, lam_exept:float=0):

        if CryptoDataAnalysis._unique_count(prices) <= 1:
            raise ValueError("Data must not be constant. Please check your data.")

        transformed, lam = boxcox(prices)
        if CryptoDataAnalysis._unique_count(transformed) <= 1:
            lam = lam_exept
            transformed = boxcox(prices, lmbda=lam)
            model = SARIMAX(transformed, order=(2, 1, 2), seasonal_order=(1, 1, 1, season))

            results = model.fit(disp=False)
            forecast = results.get_forecast(steps=steps)
            forecast_transformed = forecast.predicted_mean
            forecast_values = CryptoDataAnalysis._inv_boxcox(forecast_transformed, lam)
        else:
            try:
                model = SARIMAX(transformed, order=(2, 1, 2), seasonal_order=(1, 1, 1, season))
            except Exception:
                lam = lam_exept
                transformed = boxcox(prices, lmbda=lam)
                model = SARIMAX(transformed, order=(2, 1, 2), seasonal_order=(1, 1, 1, season))

            results = model.fit(disp=False)
            forecast = results.get_forecast(steps=steps)
            forecast_transformed = forecast.predicted_mean
            forecast_values = CryptoDataAnalysis._inv_boxcox(forecast_transformed, lam)

        return forecast_values

    def predict_prices(self):
        season, predict_end_delta = self._determine_time_step()
        future_steps = predict_end_delta // (self._data.index[1] - self._data.index[0])

        close_predictions = self.get_sarimax_predict(self.data['close'], future_steps, season )
        high_predictions = self.get_sarimax_predict(self.data['high'], future_steps, season )
        low_predictions = self.get_sarimax_predict(self.data['low'], future_steps, season)

        open_predictions = np.empty(future_steps)
        open_predictions[0] = self._data['close'].iloc[-1]
        open_predictions[1:] = close_predictions[:-1]

        self._data_predictions = pd.DataFrame({
            'close_predictions': close_predictions,
            'open_predictions': open_predictions,
            'high_predictions': high_predictions,
            'low_predictions': low_predictions
        })

    def plot(self):
        plt.figure(figsize=(14, 7))

        for i in range(len(self._data)):
            line_color = 'red' if self._data['close'].iloc[i] < self._data['open'].iloc[i] else 'green'
            plt.plot([self._data.index[i], self._data.index[i]],
                     [self._data['low'].iloc[i], self._data['high'].iloc[i]], color=line_color, linewidth=0.5)
            plt.plot([self._data.index[i], self._data.index[i]],
                     [self._data['open'].iloc[i], self._data['close'].iloc[i]], color=line_color, linewidth=3)

        plt.fill_between(self._data.index, self._data['low'], self._data['high'], color='lightgray', alpha=0.3)

        plt.scatter([], [], color='red', label='Closing < Opening (Decrease)', s=40)
        plt.scatter([], [], color='green', label='Closing >= Opening (Increase)', s=40)

        if self._data_predictions is not None:
            delta = self._data.index[1] - self._data.index[0]
            for i in range(len(self._data_predictions)):
                line_color = 'lightcoral' if self._data_predictions['close_predictions'].iloc[i] < \
                                             self._data_predictions['open_predictions'].iloc[i] else 'lightgreen'
                future_time = self._data.index[-1] + (i + 1) * delta

                plt.plot([future_time, future_time],
                         [self._data_predictions['low_predictions'].iloc[i],
                          self._data_predictions['high_predictions'].iloc[i]],
                         color=line_color, linewidth=0.5)
                plt.plot([future_time, future_time],
                         [self._data_predictions['open_predictions'].iloc[i],
                          self._data_predictions['close_predictions'].iloc[i]],
                         color=line_color, linewidth=3)

        if self._sar is not None:
            plt.scatter(self._data.index, self._sar, label='SAR', color='purple', marker='o', s=5)

        plt.title(self._crypto_name)
        plt.xlabel('Date')
        plt.ylabel('Price')
        plt.legend()
        plt.grid()
        plt.show()

    @property
    def data(self):
        return self._data

    @property
    def data_predictions(self):
        return self._data_predictions

    @property
    def sar(self):
        return self._sar

    @property
    def crypto_name(self):
        return self._crypto_name
