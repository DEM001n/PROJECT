import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
from sklearn.datasets import make_classification
from sklearn.model_selection import train_test_split
from sklearn.multiclass import OneVsOneClassifier, OneVsRestClassifier
from sklearn.linear_model import LogisticRegression, Perceptron
from sklearn import svm
from sklearn import metrics
from sklearn.preprocessing import label_binarize

if __name__ == '__main__':
    X, y = make_classification(n_samples=2000, n_features=2, n_informative=2,
                               n_redundant=0, n_classes=4, n_clusters_per_class=1, random_state=42)

    X_train, X_test, y_train, y_test = train_test_split(X, y, test_size=0.5, random_state=42)

    base_classifiers = [
        svm.SVC(kernel='linear', probability=True),
        svm.SVC(kernel='rbf', probability=True),
        LogisticRegression(),
        Perceptron()
    ]

    strategies = {'OvO': OneVsOneClassifier, 'OvR': OneVsRestClassifier}

    results = []

    for clf in base_classifiers:
        clf_name = clf.__class__.__name__ + (f"_{clf.kernel}" if hasattr(clf, "kernel") else "") #

        for strategy_name, strategy in strategies.items():

            clf_wrapped = strategy(clf)
            clf_wrapped.fit(X_train, y_train)
            y_pred = clf_wrapped.predict(X_test)

            fig, axes = plt.subplots(1, 3, figsize=(18, 5))
            fig.suptitle(f"{clf_name} - {strategy_name}", fontsize=16)

            scatter1 = axes[0].scatter(X_test[:, 0], X_test[:, 1], c=y_test, cmap='tab10', s=10)
            axes[0].set_title("Rzeczywiste etykiety")

            scatter2 = axes[1].scatter(X_test[:, 0], X_test[:, 1], c=y_pred, cmap='tab10', s=10)
            axes[1].set_title("Predykcja klasyfikatora")

            colors = np.where(y_pred == y_test, 'green', 'red')
            scatter = axes[2].scatter(X_test[:, 0], X_test[:, 1], c=colors, s=10)
            axes[2].set_title("Różnice (zielone = poprawne, czerwone = błędne)")

            plt.tight_layout()
            plt.show()



            accuracy = metrics.accuracy_score(y_test, y_pred)
            recall = metrics.recall_score(y_test, y_pred, average='macro')
            precision = metrics.precision_score(y_test, y_pred, average='macro')
            f1 = metrics.f1_score(y_test, y_pred, average='macro')

            print(f"Dokładność (Accuracy): {accuracy:.4f}")
            print(f"Czułość (Recall): {recall:.4f}")
            print(f"Precyzja (Precision): {precision:.4f}")
            print(f"F1 Score: {f1:.4f}")

            try:
                y_test_bin = label_binarize(y_test, classes=np.unique(y))

                if hasattr(clf_wrapped, "predict_proba"):
                    y_scores = clf_wrapped.predict_proba(X_test)
                elif hasattr(clf_wrapped, "decision_function"):
                    y_scores = clf_wrapped.decision_function(X_test)

                    if y_scores.ndim == 1:
                        y_scores = np.expand_dims(y_scores, axis=1)
                else:
                    raise AttributeError()

                auc_macro = metrics.roc_auc_score(y_test_bin, y_scores, average='macro', multi_class='ovr')
                print(f"Średnie AUC (macro): {auc_macro:.4f}\n")

                fpr = dict()
                tpr = dict()
                roc_auc = dict()
                n_classes = y_test_bin.shape[1]

                plt.figure(figsize=(8, 6))
                for i in range(n_classes):
                    fpr[i], tpr[i], _ = metrics.roc_curve(y_test_bin[:, i], y_scores[:, i])
                    roc_auc[i] = metrics.auc(fpr[i], tpr[i])
                    plt.plot(fpr[i], tpr[i], lw=2, label=f"Klasa {i} (AUC = {roc_auc[i]:.2f})")

                plt.plot([0, 1], [0, 1], 'k--', lw=1)
                plt.xlabel("False Positive Rate")
                plt.ylabel("True Positive Rate")
                plt.title("Krzywa ROC dla każdej klasy")
                plt.legend(loc="lower right")
                plt.grid()
                plt.show()

            except Exception as e:
                print()
                auc_macro = 0

            results.append({
                "Model": clf_name,
                "Strategia": strategy_name,
                "Accuracy": accuracy,
                "Recall": recall,
                "Precision": precision,
                "F1": f1,
                "ROC AUC": auc_macro
            })

            xx, yy = np.meshgrid(np.linspace(X[:, 0].min() - 0.5, X[:, 0].max() + 0.5, 300),
                                 np.linspace(X[:, 1].min() - 0.5, X[:, 1].max() + 0.5, 300))
            grid = np.c_[xx.ravel(), yy.ravel()]
            Z = clf_wrapped.predict(grid).reshape(xx.shape)

            plt.figure(figsize=(8, 6))
            plt.contourf(xx, yy, Z, alpha=0.3, cmap='tab10')
            plt.scatter(X_test[:, 0], X_test[:, 1], c=y_test, cmap='tab10', s=10, edgecolor='k')
            plt.title(f"Granica decyzyjna - {clf_name} ({strategy_name})")
            plt.xlabel("X1")
            plt.ylabel("X2")
            plt.show()

    df = pd.DataFrame(results)

    metrics_to_plot = ["Accuracy", "Recall", "Precision", "F1", "ROC AUC"]
    fig, ax = plt.subplots(figsize=(14, 6))

    df["Grupa"] = df["Model"] + " (" + df["Strategia"] + ")"
    x = np.arange(len(df["Grupa"]))
    width = 0.15

    for i, metric in enumerate(metrics_to_plot):
        values = df[metric].fillna(0)
        ax.bar(x + i * width, values, width, label=metric)

    ax.set_ylabel("Wartość")
    ax.set_title("Porównanie jakości klasyfikatorów")
    ax.set_xticks(x + width * (len(metrics_to_plot) - 1) / 2)
    ax.set_xticklabels(df["Grupa"], rotation=45, ha="right")
    ax.legend()
    plt.tight_layout()
    plt.show()

'''
One-vs-One (OvO): każdy klasyfikator rozróżnia między dwiema klasami.
One-vs-Rest (OvR): każdy klasyfikator odróżnia jedną klasę od reszty.
Niektóre klasyfikatory lepiej działają z OvR (np. Perceptron, SVC_rbf), ale różnice są niewielkie – rzędu 1–2 punktów procentowych.
OvR ma często wyższy AUC, co sugeruje, że lepiej radzi sobie w sytuacjach, gdy klasy są niesymetryczne lub gdy istnieje klasa dominująca.
Obie strategie mogą być skuteczne, ale OvR może lepiej uchwycić ogólne separacje klas w danych, zwłaszcza w kontekście AUC.

SVC_linear - Liniowa
SVC_rbf	- Nieliniowa
LogisticRegression - Liniowa
Perceptron - Liniowa
SVC(kernel='linear'), LogisticRegression, Perceptron tworzą proste linie podziału.
SVC(kernel='rbf') używa jądra Gaussa – tworzy zakrzywione, adaptacyjne granice.
Modele liniowe są prostsze, ale mogą nie uchwycić bardziej złożonych rozkładów klas — wtedy lepiej działa model nieliniowy (SVC_rbf).

AUC mierzy zdolność modelu do prawidłowego rozróżniania klas. Wysokie AUC oznacza, że model potrafi dobrze oddzielić jedną klasę od innych.
AUC zależy nie tylko od modelu, ale również od tego, jak klasy są rozmieszczone w przestrzeni cech. Im bardziej rozproszone lub zbliżone do innych klas, tym trudniejsze rozróżnienie i niższe AUC.
'''