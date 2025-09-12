import numpy as np
import matplotlib.pyplot as plt
import pandas as pd
from sklearn.datasets import make_classification
from sklearn.model_selection import train_test_split, GridSearchCV
from sklearn.naive_bayes import GaussianNB
from sklearn.discriminant_analysis import QuadraticDiscriminantAnalysis
from sklearn.neighbors import KNeighborsClassifier
from sklearn import svm
from sklearn.tree import DecisionTreeClassifier
from sklearn import metrics
import time

def zad1():
    #============================ zad.1 ============================
    X, y = make_classification(n_samples=500, n_features=2, n_classes=2,
                               n_clusters_per_class=2, n_redundant=0,random_state=4)

    #============================ zad.2 ============================
    plt.figure(figsize=(6, 5))
    plt.scatter(X[:, 0], X[:, 1], c=y, cmap='bwr', edgecolor='k', s=20)
    plt.title("Wygenerowane dane - make_classification")
    plt.xlabel("Cecha 1")
    plt.ylabel("Cecha 2")
    plt.show()

    #============================ zad.3  ============================
    classifiers = {
        "GaussianNB": GaussianNB(),
        "QDA": QuadraticDiscriminantAnalysis(),
        "KNN": KNeighborsClassifier(),
        "SVC": svm.SVC(probability=True),
        "DecisionTree": DecisionTreeClassifier()
    }

    #============================ zad.4 ============================
    results = []

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

            metrics_list["accuracy"].append(metrics.accuracy_score(y_test, y_pred))
            metrics_list["recall"].append(metrics.recall_score(y_test, y_pred))
            metrics_list["precision"].append(metrics.precision_score(y_test, y_pred))
            metrics_list["f1"].append(metrics.f1_score(y_test, y_pred))
            metrics_list["auc"].append(metrics.roc_auc_score(y_test, y_proba))
            metrics_list["fit_time"].append(end_fit - start_fit)
            metrics_list["test_time"].append(end_pred - start_pred)

            if i == 99:
                final_clf = clf
                final_X_test, final_y_test, final_y_pred, final_y_proba = X_test, y_test, y_pred, y_proba

        avg_metrics = {k: np.mean(v) for k, v in metrics_list.items()}
        avg_metrics["Classifier"] = name
        results.append(avg_metrics)

    #============================ zad.5 ============================
    df_results = pd.DataFrame(results)
    df_results.set_index("Classifier", inplace=True)

    df_results[['accuracy', 'recall', 'precision', 'f1', 'auc','fit_time','test_time']].plot(kind='bar',
                   figsize=(12, 6), title="Porównanie klasyfikatorów")

    plt.ylabel("Wartość metryki")
    plt.show()

    #============================ zad.6 ============================
    fig, axs = plt.subplots(1, 3, figsize=(18, 5))

    axs[0].scatter(final_X_test[:, 0], final_X_test[:, 1], c=final_y_test, cmap='bwr', edgecolor='k')
    axs[0].set_title(f"Oczekiwane etykiety (Ground Truth)")
    axs[0].set_xlabel("Cecha 1")
    axs[0].set_ylabel("Cecha 2")

    axs[1].scatter(final_X_test[:, 0], final_X_test[:, 1], c=final_y_pred, cmap='bwr', edgecolor='k')
    axs[1].set_title(f"Przewidziane etykiety (Predykcja)")
    axs[1].set_xlabel("Cecha 1")
    axs[1].set_ylabel("Cecha 2")

    errors = final_y_test != final_y_pred
    axs[2].scatter(final_X_test[:, 0], final_X_test[:, 1], c=errors, cmap='coolwarm', edgecolor='k')
    axs[2].set_title(f"Błędy klasyfikacji (Różnice)")
    axs[2].set_xlabel("Cecha 1")
    axs[2].set_ylabel("Cecha 2")
    plt.show()

    #============================ zad.7 ============================
    fpr, tpr, _ = metrics.roc_curve(final_y_test, final_y_proba)
    plt.figure(figsize=(6, 5))
    plt.plot(fpr, tpr, label=f"AUC = {metrics.roc_auc_score(final_y_test, final_y_proba):.2f}")
    plt.plot([0, 1], [0, 1], 'k--')
    plt.xlabel("False Positive Rate")
    plt.ylabel("True Positive Rate")
    plt.title(f"Krzywa ROC - {name}")
    plt.legend(loc="lower right")
    plt.show()

    #============================ zad.8 ============================
    xx, yy = np.meshgrid(np.linspace(X[:, 0].min(), X[:, 0].max(), 100),
                         np.linspace(X[:, 1].min(), X[:, 1].max(), 100))
    grid = np.c_[xx.ravel(), yy.ravel()]
    Z = final_clf.predict(grid).reshape(xx.shape)

    plt.figure(figsize=(6, 5))
    plt.contourf(xx, yy, Z, alpha=0.3, cmap='bwr')
    plt.scatter(X[:, 0], X[:, 1], c=y, cmap='bwr', edgecolor='k', s=20)
    plt.title(f"Granica decyzyjna - {name}")
    plt.xlabel("Cecha 1")
    plt.ylabel("Cecha 2")
    plt.show()



def zad2():
    #============================ zad.1 ============================
    X, y = make_classification(n_samples=500, n_features=2, n_classes=2,
                               n_clusters_per_class=1, n_redundant=0, random_state=42)

    #============================ zad.2 ============================
    svc = svm.SVC(probability=True)

    #============================ zad.3 ============================
    C_range = [0.001, 0.1, 10, 1000, 100000]
    gamma_range = [0.001, 0.01, 0.1, 1]
    param_grid = {'C': C_range, 'gamma': gamma_range}

    #============================ zad.4 ============================
    grid = GridSearchCV(svc, param_grid=param_grid, scoring='roc_auc', cv=5)
    grid.fit(X, y)
    best_params = grid.best_params_
    print("Najlepsze parametry:", best_params)

    #============================ zad.5 ============================
    results = pd.DataFrame(grid.cv_results_)
    auc_matrix = results.pivot(index='param_gamma', columns='param_C', values='mean_test_score').values

    plt.figure(figsize=(6, 5))
    plt.imshow(auc_matrix, interpolation='nearest', cmap='viridis', aspect='auto', origin='lower')
    plt.colorbar(label='ROC AUC')
    plt.xticks(np.arange(len(C_range)), C_range)
    plt.yticks(np.arange(len(gamma_range)), gamma_range)
    plt.xlabel("Parametr C")
    plt.ylabel("Parametr gamma")
    plt.title("AUC - mapa kolorów (imshow)")
    plt.show()

    #============================ zad.6 ============================
    metrics_list = {
        "accuracy": [], "recall": [], "precision": [],
        "f1": [], "auc": [], "fit_time": [], "test_time": []
    }

    final_model = svm.SVC(probability=True, **best_params)

    for i in range(100):
        X_train, X_test, y_train, y_test = train_test_split(X, y, test_size=0.3)

        start_fit = time.time()
        final_model.fit(X_train, y_train)
        end_fit = time.time()

        start_pred = time.time()
        y_pred = final_model.predict(X_test)
        y_proba = final_model.predict_proba(X_test)[:, 1]
        end_pred = time.time()

        metrics_list["accuracy"].append(metrics.accuracy_score(y_test, y_pred))
        metrics_list["recall"].append(metrics.recall_score(y_test, y_pred))
        metrics_list["precision"].append(metrics.precision_score(y_test, y_pred))
        metrics_list["f1"].append(metrics.f1_score(y_test, y_pred))
        metrics_list["auc"].append(metrics.roc_auc_score(y_test, y_proba))
        metrics_list["fit_time"].append(end_fit - start_fit)
        metrics_list["test_time"].append(end_pred - start_pred)

        if i == 99:
            final_X_test, final_y_test, final_y_pred, final_y_proba = X_test, y_test, y_pred, y_proba

    #============================ zad.8 ============================
    df_metrics = pd.DataFrame(metrics_list)
    avg_metrics = df_metrics.mean()
    print("\nŚrednie metryki klasyfikacji:\n", avg_metrics)

    #============================ zad.9 ============================
    fpr, tpr, _ = metrics.roc_curve(final_y_test, final_y_proba)
    plt.figure(figsize=(6, 5))
    plt.plot(fpr, tpr, label=f"AUC = {metrics.roc_auc_score(final_y_test, final_y_proba):.2f}")
    plt.plot([0, 1], [0, 1], 'k--')
    plt.xlabel("False Positive Rate")
    plt.ylabel("True Positive Rate")
    plt.title("Krzywa ROC")
    plt.legend()
    plt.grid()
    plt.show()

    xx, yy = np.meshgrid(np.linspace(X[:, 0].min(), X[:, 0].max(), 100),
                         np.linspace(X[:, 1].min(), X[:, 1].max(), 100))
    Z = final_model.predict(np.c_[xx.ravel(), yy.ravel()]).reshape(xx.shape)

    plt.figure(figsize=(6, 5))
    plt.contourf(xx, yy, Z, alpha=0.3, cmap='bwr')
    plt.scatter(X[:, 0], X[:, 1], c=y, cmap='bwr', edgecolor='k')
    plt.title("Granica decyzyjna (SVC)")
    plt.xlabel("Cecha 1")
    plt.ylabel("Cecha 2")
    plt.show()


if __name__ == '__main__':
    zad1()
    zad2()