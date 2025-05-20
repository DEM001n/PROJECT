from crypto_class import CryptoDataManager,CryptoDataAnalysis
import numpy as np

def main():
    name = ("btc")
    data_time = ("w")
    output_file = "crypto_data.csv"

    obj1=CryptoDataManager(output_file,name,data_time)
    obj1.get_and_save_crypto_data()

    data = CryptoDataAnalysis.load_data(output_file)
    obj2 = CryptoDataAnalysis(data)

    #obj2.calculate_sar()
    average_coefficient,average_volume,average_market_cap = obj2.get_calculate_stability_coefficient_and_info()

    print(f"Stability factor: {average_coefficient:.2f}%")
    print(f"Average volume: {average_volume:.2f}")
    print(f"Average market capitalization: {average_market_cap:.2f}")

    obj2.predict_prices()
    data_predictions = obj2.data_predictions

    print(f"Predicted closing price:\n{np.array(data_predictions['close_predictions'])}")
    print(f"Predicted opening price:\n{np.array(data_predictions['open_predictions'])}")
    print(f"Predicted high price at:\n{np.array(data_predictions['high_predictions'])}")
    print(f"Predicted low price for:\n{np.array(data_predictions['low_predictions'])}")

    obj2.plot()

if __name__ == "__main__":
    main()
