import numpy as np
import matplotlib.pyplot as plt
import pandas as pd
from sklearn.datasets import make_classification
from sklearn.model_selection import train_test_split
from sklearn.naive_bayes import GaussianNB
from sklearn.discriminant_analysis import QuadraticDiscriminantAnalysis
from sklearn.neighbors import KNeighborsClassifier
from sklearn import svm
from sklearn.tree import DecisionTreeClassifier
from sklearn import metrics
import time


def zad1():
    # 1. Generowanie danych
    X, y = make_classification(n_samples=1000, n_features=2, n_classes=2,
                               n_clusters_per_class=2, n_redundant=0, random_state=42)

    # 2. Wizualizacja danych
    plt.figure(figsize=(6, 5))
    plt.scatter(X[:, 0], X[:, 1], c=y, cmap='bwr', edgecolor='k', s=20)
    plt.title("Wygenerowane dane - make_classification")
    plt.xlabel("Cecha 1")
    plt.ylabel("Cecha 2")
    plt.grid(True)
    plt.show()

    # 3. Lista klasyfikatorów
    classifiers = {
        "GaussianNB": GaussianNB(),
        "QDA": QuadraticDiscriminantAnalysis(),
        "KNN": KNeighborsClassifier(),
        "SVC": svm.SVC(probability=True),
        "DecisionTree": DecisionTreeClassifier()
    }

    results = []

    # Pętla po klasyfikatorach
    for name, clf in classifiers.items():
        metrics_list = {
            "accuracy": [],
            "recall": [],
            "precision": [],
            "f1": [],
            "auc": [],
            "fit_time": [],
            "test_time": []
        }

        for i in range(100):
            X_train, X_test, y_train, y_test = train_test_split(X, y, test_size=0.3)

            start_fit = time.time()
            clf.fit(X_train, y_train)
            end_fit = time.time()

            start_pred = time.time()
            y_pred = clf.predict(X_test)
            end_pred = time.time()

            y_proba = clf.predict_proba(X_test)[:, 1] if hasattr(clf, "predict_proba") else np.zeros_like(y_test)

            # Metryki
            metrics_list["accuracy"].append(metrics.accuracy_score(y_test, y_pred))
            metrics_list["recall"].append(metrics.recall_score(y_test, y_pred))
            metrics_list["precision"].append(metrics.precision_score(y_test, y_pred))
            metrics_list["f1"].append(metrics.f1_score(y_test, y_pred))
            metrics_list["auc"].append(metrics.roc_auc_score(y_test, y_proba))
            metrics_list["fit_time"].append(end_fit - start_fit)
            metrics_list["test_time"].append(end_pred - start_pred)

            # Zapamiętanie ostatniej iteracji
            if i == 99:
                final_clf = clf
                final_X_test, final_y_test, final_y_pred, final_y_proba = X_test, y_test, y_pred, y_proba

        # Średnie metryki
        avg_metrics = {k: np.mean(v) for k, v in metrics_list.items()}
        avg_metrics["Classifier"] = name
        results.append(avg_metrics)

    # 4. Tabela wyników
    df_results = pd.DataFrame(results)
    df_results.set_index("Classifier", inplace=True)
    print(df_results)

    # 5. Wykresy porównujące klasyfikatory
    df_results[['accuracy', 'recall', 'precision', 'f1', 'auc']].plot(kind='bar', figsize=(12, 6), title="Porównanie klasyfikatorów")
    plt.ylabel("Wartość metryki")
    plt.grid(axis='y')
    plt.show()

    # 6. Błędy klasyfikacji (ostatnia iteracja)
    plt.figure(figsize=(6, 5))
    plt.scatter(final_X_test[:, 0], final_X_test[:, 1], c=final_y_test != final_y_pred, cmap='coolwarm', edgecolor='k')
    plt.title(f"Błędy klasyfikacji - {name}")
    plt.xlabel("Cecha 1")
    plt.ylabel("Cecha 2")
    plt.grid(True)
    plt.show()

    # 7. Krzywa ROC
    fpr, tpr, thresholds = metrics.roc_curve(final_y_test, final_y_proba)
    plt.figure(figsize=(6, 5))
    plt.plot(fpr, tpr, label=f"AUC = {metrics.roc_auc_score(final_y_test, final_y_proba):.2f}")
    plt.plot([0, 1], [0, 1], 'k--')
    plt.xlabel("False Positive Rate")
    plt.ylabel("True Positive Rate")
    plt.title(f"Krzywa ROC - {name}")
    plt.legend(loc="lower right")
    plt.grid(True)
    plt.show()

    # 8. Granica decyzyjna
    xx, yy = np.meshgrid(np.linspace(X[:, 0].min(), X[:, 0].max(), 300),
                         np.linspace(X[:, 1].min(), X[:, 1].max(), 300))
    grid = np.c_[xx.ravel(), yy.ravel()]
    Z = final_clf.predict(grid).reshape(xx.shape)

    plt.figure(figsize=(6, 5))
    plt.contourf(xx, yy, Z, alpha=0.3, cmap='bwr')
    plt.scatter(X[:, 0], X[:, 1], c=y, cmap='bwr', edgecolor='k', s=20)
    plt.title(f"Granica decyzyjna - {name}")
    plt.xlabel("Cecha 1")
    plt.ylabel("Cecha 2")
    plt.grid(True)
    plt.show()

if __name__ == '__main__':
    zad1()