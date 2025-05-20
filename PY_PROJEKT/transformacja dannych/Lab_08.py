import numpy as np
import matplotlib.pyplot as plt
from sklearn import datasets
from sklearn.model_selection import LeaveOneOut
from collections import Counter
from sklearn.neighbors import KDTree
from sklearn.model_selection import KFold
from sklearn.metrics import mean_squared_error
from sklearn.datasets import fetch_california_housing #load_boston jest usunięte z najnowszych wersji sklearn. Alternatywa: fetch_california_housing
import time

class KNN:
    def __init__(self, n_neighbors=1, use_KDTree=False, task_type='classification'):
        self.k = n_neighbors
        self.use_KDTree = use_KDTree
        self.task_type = task_type
        self.X_train = None
        self.y_train = None
        self.kdtree = None

    def fit(self, X, y):
        self.X_train = np.array(X)
        self.y_train = np.array(y)
        if self.use_KDTree:
            self.kdtree = KDTree(self.X_train)

    def predict(self, X):
        X = np.array(X)
        predictions = []

        if self.use_KDTree:
            dists, indices = self.kdtree.query(X, k=self.k)
        else:
            indices = []
            for x in X:
                dists = np.linalg.norm(self.X_train - x, axis=1)
                nearest = np.argsort(dists)[:self.k]
                indices.append(nearest)

        for neighbors in indices:
            neighbor_labels = self.y_train[neighbors]
            if self.task_type == 'classification':
                most_common = Counter(neighbor_labels).most_common(1)[0][0]
                predictions.append(most_common)
            else:
                predictions.append(np.mean(neighbor_labels))

        return np.array(predictions)

    def score(self, X, y):
        y_pred = self.predict(X)
        if self.task_type == 'classification':
            return np.mean(y_pred == y) * 100
        else:
            return np.mean((y_pred - y) ** 2)

def wiPCA(X, n_components):
    mean_vector = np.mean(X, axis=0)
    X_centered = X - mean_vector
    cov_matrix = np.cov(X_centered.T)
    eig_vals, eig_vecs = np.linalg.eigh(cov_matrix)
    sorted_idx = np.argsort(eig_vals)[::-1]
    eig_vecs = eig_vecs[:, sorted_idx]
    selected_vectors = eig_vecs[:, :n_components]
    X_reduced = X_centered @ selected_vectors
    return X_reduced, selected_vectors, eig_vals, mean_vector

def wiPCA_inverse(X_reduced, selected_vectors, mean_vector):
    return X_reduced @ selected_vectors.T + mean_vector

def zad1():
    # -------------------- Zadanie 1 --------------------
    X, y = datasets.make_classification(
        n_samples=100,
        n_features=2,
        n_informative=2,
        n_redundant=0,
        n_repeated=0,
        random_state=3
    )

    # -------------------- Zadanie 2 --------------------
    model = KNN(n_neighbors=1, use_KDTree=False)
    model.fit(X, y)

    # -------------------- Zadanie 3 --------------------
    h = 0.1
    x_min, x_max = X[:, 0].min() - 1, X[:, 0].max() + 1
    y_min, y_max = X[:, 1].min() - 1, X[:, 1].max() + 1
    xx, yy = np.meshgrid(np.arange(x_min, x_max, h),
                         np.arange(y_min, y_max, h))
    grid_points = np.c_[xx.ravel(), yy.ravel()]
    Z = model.predict(grid_points).reshape(xx.shape)

    plt.figure(figsize=(8, 6))
    plt.contourf(xx, yy, Z, cmap=plt.cm.coolwarm, alpha=0.3)
    plt.scatter(X[:, 0], X[:, 1], c=y, cmap=plt.cm.coolwarm, edgecolors='k')
    plt.title("Granica decyzyjna dla danych 2D (k=1)")
    plt.xlabel("Cecha 1")
    plt.ylabel("Cecha 2")
    plt.grid()
    plt.show()

    # -------------------- Zadanie 4 --------------------
    iris = datasets.load_iris()
    X_iris = iris.data
    y_iris = iris.target
    
    knn_iris = KNN(n_neighbors=1, use_KDTree=True)
    knn_iris.fit(X_iris, y_iris)
    
    # -------------------- Zadanie 5 --------------------
    X_pca, vecs, _, mean_vec = wiPCA(X_iris, 2)
    
    x_min, x_max = X_pca[:, 0].min() - 1, X_pca[:, 0].max() + 1
    y_min, y_max = X_pca[:, 1].min() - 1, X_pca[:, 1].max() + 1
    xx, yy = np.meshgrid(np.arange(x_min, x_max, 0.1),
                         np.arange(y_min, y_max, 0.1))
    grid_pca = np.c_[xx.ravel(), yy.ravel()]
    grid_original = wiPCA_inverse(grid_pca, vecs, mean_vec)
    Z = knn_iris.predict(grid_original).reshape(xx.shape)
    
    plt.figure(figsize=(8, 6))
    plt.contourf(xx, yy, Z, cmap=plt.cm.coolwarm, alpha=0.3)
    plt.scatter(X_pca[:, 0], X_pca[:, 1], c=y_iris, cmap=plt.cm.coolwarm, edgecolors='k')
    plt.title("Granica decyzyjna Iris (PCA + kNN)")
    plt.xlabel("PCA1")
    plt.ylabel("PCA2")
    plt.grid()
    plt.show()

    # -------------------- Zadanie 6 --------------------
    loo = LeaveOneOut()
    k_values = range(1, 11)
    results = []
    
    for k in k_values:
        errors = []
        for train_index, test_index in loo.split(X_iris):
            X_train, X_test = X_iris[train_index], X_iris[test_index]
            y_train, y_test = y_iris[train_index], y_iris[test_index]
            model = KNN(n_neighbors=k)
            model.fit(X_train, y_train)
            pred = model.predict(X_test)
            errors.append(pred[0] != y_test[0])
        error_rate = np.mean(errors)
        results.append((k, 1 - error_rate))
    
    print("\nLeave-One-Out Accuracy for different k values:")
    print(" k | Accuracy")
    print("---|---------")
    for k, acc in results:
        print(f"{k:2d} | {acc:.4f}")
    
    # -------------------- Zadanie 7 --------------------
    start = time.time()
    knn_basic = KNN(n_neighbors=1, use_KDTree=False)
    knn_basic.fit(X_iris, y_iris)
    knn_basic.predict(X_iris)
    t_basic = time.time() - start
    
    start = time.time()
    knn_kd = KNN(n_neighbors=1, use_KDTree=True)
    knn_kd.fit(X_iris, y_iris)
    knn_kd.predict(X_iris)
    t_kd = time.time() - start
    
    print(f"\nCzas działania brute-force: {t_basic:.6f} s")
    print(f"Czas działania z KD-Tree : {t_kd:.6f} s")

def zad2():
    # -------------------- Zadanie 1 --------------------
    X_reg, y_reg = datasets.make_regression(
        n_samples=100,
        n_features=1,
        noise=10,
        random_state=1
    )

    # -------------------- Zadanie 2 --------------------
    knn_reg = KNN(n_neighbors=5, use_KDTree=False, task_type='regression')
    knn_reg.fit(X_reg, y_reg)

    # -------------------- Zadanie 3 --------------------
    X_plot = np.linspace(X_reg.min(), X_reg.max(), 300).reshape(-1, 1)
    y_pred = knn_reg.predict(X_plot)

    plt.figure(figsize=(8, 6))
    plt.scatter(X_reg, y_reg, color='blue', label='Dane uczące')
    plt.plot(X_plot, y_pred, color='red', label='Regresja k-NN')
    plt.title("Regresja k-NN (k=5)")
    plt.xlabel("X")
    plt.ylabel("y")
    plt.legend()
    plt.grid()
    plt.show()

    mse = knn_reg.score(X_reg, y_reg)
    print(f"Błąd średniokwadratowy (MSE): {mse:.2f}")

    # -------------------- Zadanie 4 --------------------
    boston = fetch_california_housing()
    X_boston = boston.data
    y_boston = boston.target

    # -------------------- Zadanie 5 --------------------
    k_values = range(1, 11)
    kf = KFold(n_splits=10, shuffle=True, random_state=42)
    results = []

    for k in k_values:
        mse_scores = []
        for train_idx, test_idx in kf.split(X_boston):
            X_train, X_test = X_boston[train_idx], X_boston[test_idx]
            y_train, y_test = y_boston[train_idx], y_boston[test_idx]
            model = KNN(n_neighbors=k,use_KDTree=True,task_type='regression')
            model.fit(X_train, y_train)
            y_pred = model.predict(X_test)
            mse = mean_squared_error(y_test, y_pred)
            mse_scores.append(mse)
        avg_mse = np.mean(mse_scores)
        results.append((k, avg_mse))

    print("\n10-krotna walidacja krzyżowa – MSE dla różnych k:")
    print(" k | Średni Błąd MSE")
    print("---|----------------")
    for k, mse in results:
        print(f"{k:2d} | {mse:.4f}")

if __name__ == "__main__":
    #zad1()
    zad2()