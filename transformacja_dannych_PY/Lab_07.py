import numpy as np
import matplotlib.pyplot as plt
from sklearn.datasets import load_iris
from sklearn.datasets import load_digits


def wiPCA(X, n_components):
    mean_vector = np.mean(X, axis=0)
    X_centered = X - mean_vector
    cov_matrix = np.cov(X_centered.T)
    eig_vals, eig_vecs = np.linalg.eigh(cov_matrix)

    sorted_idx = np.argsort(eig_vals)[::-1]
    eig_vals = eig_vals[sorted_idx]
    eig_vecs = eig_vecs[:, sorted_idx]

    selected_vectors = eig_vecs[:, :n_components]
    X_reduced = X_centered @ selected_vectors

    return X_reduced, selected_vectors, eig_vals, mean_vector

def wiPCA_inverse(X_reduced, selected_vectors, mean_vector):
    return X_reduced @ selected_vectors.T + mean_vector

def zad1():
    np.random.seed(0)
    data = np.dot(np.random.rand(2, 2), np.random.randn(2, 200)).T

    X_reduced, eig_vecs, eig_vals, mean_vec = wiPCA(data, n_components=2)

    first_pc = eig_vecs[:, 0]
    second_pc = eig_vecs[:, 1]
    scaled_vec1 = first_pc * 3
    scaled_vec2 = second_pc * 3

    projections = (data - mean_vec) @ first_pc[:, np.newaxis]
    projected_points = projections * first_pc + mean_vec

    plt.figure(figsize=(6, 6))
    plt.scatter(data[:, 0], data[:, 1], alpha=0.4, color='green', label="Oryginalne dane")
    plt.scatter(projected_points[:, 0], projected_points[:, 1], color='red', s=10, label='Rzut na 1. składową')
    plt.quiver(*mean_vec, *scaled_vec1, angles='xy', scale_units='xy', scale=1, color='black')
    plt.quiver(*mean_vec, *scaled_vec2, angles='xy', scale_units='xy', scale=1, color='black')
    plt.legend()
    plt.title("Rzut danych na pierwszą składową")
    plt.xlabel("X1")
    plt.ylabel("X2")
    plt.grid(True)
    plt.axis("equal")
    plt.show()

def zad2():
    iris = load_iris()
    X = iris.data
    y = iris.target
    labels = iris.target_names

    X_reduced, eig_vecs, eig_vals, mean_vec = wiPCA(X, n_components=2)

    plt.figure(figsize=(8, 6))
    for class_idx in np.unique(y):
        plt.scatter(X_reduced[y == class_idx, 0],X_reduced[y == class_idx, 1],label=labels[class_idx],alpha=0.7)

    plt.title("Iris – PCA (2D)")
    plt.xlabel("Główna składowa 1")
    plt.ylabel("Główna składowa 2")
    plt.legend()
    plt.grid(True)
    plt.axis("equal")
    plt.show()

def zad3():
    digits = load_digits()
    X = digits.data
    y = digits.target

    X_reduced_2D, vectors_2D, eigvals, mean_vec = wiPCA(X, n_components=2)

    explained_variance = eigvals / np.sum(eigvals)
    cumulative_variance = np.cumsum(explained_variance)

    plt.figure(figsize=(8, 5))
    plt.plot(np.arange(1, len(cumulative_variance) + 1), cumulative_variance)
    plt.title("Skumulowana wariancja wyjaśniona przez PCA")
    plt.xlabel("Liczba składowych głównych")
    plt.ylabel("Skumulowana wariancja")
    plt.grid(True)
    plt.show()

    plt.figure(figsize=(10, 6))
    for digit in np.unique(y):
        plt.scatter(X_reduced_2D[y == digit, 0], X_reduced_2D[y == digit, 1], label=str(digit), s=15, alpha=0.6)
    plt.legend()
    plt.title("Digits – PCA redukcja do 2D")
    plt.xlabel("PCA 1")
    plt.ylabel("PCA 2")
    plt.grid(True)
    plt.show()

    errors = []
    components_range = range(1, 65)

    for n in components_range:
        X_red, vecs, _, mean_vec = wiPCA(X, n_components=n)
        X_rec = wiPCA_inverse(X_red, vecs, mean_vec)
        mse = np.mean((X - X_rec) ** 2)
        errors.append(mse)

    plt.figure(figsize=(8, 5))
    plt.plot(components_range, errors)
    plt.title("Średni błąd rekonstrukcji w zależności od liczby składowych")
    plt.xlabel("Liczba składowych głównych")
    plt.ylabel("MSE (średni błąd)")
    plt.grid(True)
    plt.show()

if __name__ == "__main__":
    #zad1()
    #zad2()
    zad3()