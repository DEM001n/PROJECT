import pandas as pd
import numpy as np
from numpy.linalg import inv
import matplotlib.pyplot as plt
from sklearn.decomposition import PCA



def distp(X, C):
    n = X.shape[0]
    k = C.shape[0]
    D = np.zeros((n, k))
    for i in range(n):
        for j in range(k):
            D[i, j] = np.linalg.norm(X[i] - C[j])
    return D

def distm(X, C, V):
    V_inv = inv(V)
    n = X.shape[0]
    k = C.shape[0]
    D = np.zeros((n, k))
    for i in range(n):
        for j in range(k):
            diff = X[i] - C[j]
            D[i, j] = np.sqrt(diff @ V_inv @ diff.T)
    return D

def ksrodki(X, k, metoda='euklides'):
    idx = np.random.choice(X.shape[0], size=k, replace=False)
    C = X[idx, :].copy()

    for _ in range(1000):
        if metoda == 'euklides':
            D = distp(X, C)
        elif metoda == 'mahalanobis':
            V = np.cov(X, rowvar=False)
            D = distm(X, C, V)
        else:
            raise ValueError("Nieznana metoda: użyj 'euklides' lub 'mahalanobis'")

        CX = np.argmin(D, axis=1)

        nowy_C = np.zeros_like(C)
        for i in range(k):
            punkty_w_klastrze = X[CX == i]
            if len(punkty_w_klastrze) > 0:
                nowy_C[i] = np.mean(punkty_w_klastrze, axis=0)
            else:
                nowy_C[i] = X[np.random.randint(0, X.shape[0])]

        if np.allclose(C, nowy_C):
            break
        C = nowy_C

    return C, CX

def jakosc_grupowania(X, C, CX, k, metoda='euklides'):

    if metoda == 'euklides':
        D = distp(X, C)
    elif metoda == 'mahalanobis':
        V = np.cov(X, rowvar=False)
        D = distm(X, C, V)
    else:
        raise ValueError("Nieznana metoda: użyj 'euklides' lub 'mahalanobis'")

    numerator = 0
    denominator = 0
    for k1 in range(k):
        for k2 in range(k):
            if k1 != k2:
                dist_centroids = np.linalg.norm(C[k1] - C[k2])
                numerator += np.sum(D[CX == k1, k2]) * dist_centroids

    for i in range(k):
        dist_points_to_centroid = np.sum(D[CX == i, i] ** 2)
        denominator += dist_points_to_centroid

    return numerator / denominator if denominator != 0 else float('inf')



if __name__ == "__main__":
    df = pd.read_csv('autos.csv')

    numeric_columns = [
        'normalized-losses', 'wheel-base', 'length', 'width', 'height',
        'curb-weight', 'engine-size', 'bore', 'stroke', 'compression-ratio',
        'horsepower', 'peak-rpm', 'city-mpg', 'highway-mpg', 'price'
    ]

    for col in numeric_columns:
        if df[col].isnull().sum() > 0:
            mean_val = df[col].mean()
            df[col] = df[col].fillna(mean_val)

    text_columns = df.select_dtypes(include=['object']).columns

    for col in text_columns:
        unique_vals = df[col].dropna().unique()
        mapping = {val: idx for idx, val in enumerate(unique_vals)}
        df[col] = df[col].map(mapping)

    X = df.values
    k = 2
    powtorzenia = 5

    C_euk = []
    CX_euk = []

    for _ in range(powtorzenia):
        C, CX = ksrodki(X, k, metoda='euklides')
        C_euk.append(C)
        CX_euk.append(CX)

    C_mah = []
    CX_mah = []

    for i in range(powtorzenia):
        C, CX = ksrodki(X, k, metoda='mahalanobis')
        C_mah.append(C)
        CX_mah.append(CX)

        quality = jakosc_grupowania(X, C, CX, k, metoda='mahalanobis')
        print(f'Jakość grupowania dla próby {i + 1} (mahalanobis): {quality}')

    print()

    '''
    for C,CX in zip(C_euk,CX_euk):
        pca = PCA(n_components=2)
        X_2d = pca.fit_transform(X)
        C_2d = pca.transform(C)

        colors = ['red', 'blue']

        plt.figure(figsize=(10, 8))

        for cluster in range(k):
            idxs = (CX == cluster)
            plt.scatter(X_2d[idxs, 0], X_2d[idxs, 1],
                        label=f'Grupa {cluster}',
                        color=colors[cluster % len(colors)],alpha=0.6)

        for cluster in range(k):
            plt.scatter(C_2d[cluster, 0], C_2d[cluster, 1]
                        , marker='+', s=200, color=colors[cluster % len(colors)],
                        label=f'Centrum {cluster}')

        plt.title("Wizualizacja k-środków (euklides)")
        plt.xlabel("Pierwszy składnik PCA")
        plt.ylabel("Drugi składnik PCA")
        plt.legend()
        plt.grid(True)
        plt.show()
    '''

    for C, CX in zip(C_mah , CX_mah):
        pca = PCA(n_components=2)
        X_2d = pca.fit_transform(X)
        C_2d = pca.transform(C)

        colors = ['red', 'blue']

        plt.figure(figsize=(10, 8))

        for cluster in range(k):
            idxs = (CX == cluster)
            plt.scatter(X_2d[idxs, 0], X_2d[idxs, 1], label=f'Grupa {cluster}', color=colors[cluster % len(colors)],
                        alpha=0.6)

        for cluster in range(k):
            plt.scatter(C_2d[cluster, 0], C_2d[cluster, 1], marker='+', s=200, color=colors[cluster % len(colors)],
                        label=f'Centrum {cluster}')

        plt.title("Wizualizacja k-środków (mahalanobis)")
        plt.xlabel("Pierwszy składnik PCA")
        plt.ylabel("Drugi składnik PCA")
        plt.legend()
        plt.grid(True)
        plt.show()



    for i, (C, CX) in enumerate(zip(C_euk, CX_euk)):
        quality = jakosc_grupowania(X, C, CX, k, metoda='euklides')
        print(f'Jakość grupowania dla próby {i + 1} (euklides): {quality}')

    print()

    for i, (C, CX) in enumerate(zip(C_mah, CX_mah)):
        quality = jakosc_grupowania(X, C, CX, k, metoda='mahalanobis')
        print(f'Jakość grupowania dla próby {i + 1} (mahalanobis): {quality}')

    '''
    Potencjalne źródła problemów dla jakosci grupowania metody mahalanobis:

        trzeba dane są odpowiednio wystandaryzowane. W przeciwnym razie macierz kowariancji V 
        może mieć bardzo dużą wariancję, a odległości mogą stać się bardzo duże. 

        Jeżeli cechy w danych są silnie skorelowane, to macierz kowariancji może zawierać dużą wariancję w jednej z cech,
        a to może prowadzić do dużych odległości.
    '''