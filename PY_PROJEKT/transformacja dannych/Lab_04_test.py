from sklearn.datasets import fetch_rcv1
from sklearn.preprocessing import Binarizer
from sklearn.feature_selection import mutual_info_classif
import numpy as np

# Krok 1: Wczytanie danych
rcv1 = fetch_rcv1()
X = rcv1["data"]
Y = rcv1.target

# Krok 2: Zbinaryzowanie danych
X_bin = Binarizer().fit_transform(X)

# Krok 3: Wybór zmiennej decyzyjnej - GSPO (sport, kolumna 87)
Y_sport = Y[:, 87].toarray().flatten()

# Krok 4: Obliczenie przyrostu informacji
info_gain = mutual_info_classif(X_bin, Y_sport)

# Krok 5: Posortowanie i wypisanie 50 najważniejszych cech (indeksów)
indices = np.argsort(info_gain)[::-1]
top_50_indices = indices[:50]

print("Top 50 najbardziej informatywnych cech (indeksy):")
for i, idx in enumerate(top_50_indices, start=1):
    print(f"{i}. Feature index: {idx}, Info gain: {info_gain[idx]:.4f}")
