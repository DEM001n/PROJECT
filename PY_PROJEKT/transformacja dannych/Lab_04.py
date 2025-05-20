import numpy as np
import pandas as pd
from sklearn.datasets import fetch_rcv1
import math
import heapq
from multiprocessing import Pool


def freq(x, prob=True):
    s = pd.Series(x)
    counts = s.value_counts(normalize=prob)
    return counts.index.tolist(), counts.values.tolist()

def entropy(x):
    _, probs = freq(x, prob=True)
    return -sum(p * math.log2(p) for p in probs if p > 0)

def infogain(x, y):
    h_y = entropy(y)
    cond_entropy = 0

    xi, pi = freq(x, prob=True)

    for x_val, p_x in zip(xi, pi):
        y_given_x = y[x == x_val]
        h_y_given_x = entropy(y_given_x)
        cond_entropy += p_x * h_y_given_x

    return h_y - cond_entropy

def calculate_ig_for_range(df, y, start_idx, end_idx, top_k):
    local_heap = []
    for i in range(start_idx, end_idx):
        x_col = df.iloc[:, i]
        ig = infogain(x_col, y)

        if len(local_heap) < top_k:
            heapq.heappush(local_heap, (ig, i))
        else:
            if ig > local_heap[0][0]:
                heapq.heappushpop(local_heap, (ig, i))

    print("done!..",end = '')
    return local_heap

def merge_heaps(heap_list, top_k):
    global_heap = []

    for heap in heap_list:
        for ig, idx in heap:
            if len(global_heap) < top_k:
                heapq.heappush(global_heap, (ig, idx))
            else:
                if ig > global_heap[0][0]:
                    heapq.heappushpop(global_heap, (ig, idx))

    return global_heap

if __name__ == "__main__":
    print("Wczytywanie zbioru danych RCV1...")
    data = fetch_rcv1()

    X = data.data.copy()
    X.data = np.ones_like(X.data)

    target_names = data.target_names
    selected_tag = 'GSPO'
    tag_index = np.flatnonzero(target_names == selected_tag)[0]
    y = data.target[:, tag_index].toarray().ravel()

    print(f"Wybrany tag decyzyjny: '{selected_tag}' pozytywnych przypadków {np.sum(y)}")

    df = pd.DataFrame.sparse.from_spmatrix(X)
    df.columns = [f"word_{i}" for i in range(df.shape[1])]

    top_k = 50
    num_processes = 4

    total_features = df.shape[1]
    step = total_features // num_processes
    ranges = [(i * step, (i + 1) * step) for i in range(num_processes)]

    ranges[-1] = (ranges[-1][0], total_features)

    print(f"Obliczanie przyrostu informacji dla każdej zmiennej (słowa)...")

    with Pool(processes=num_processes) as pool:
        results = pool.starmap(calculate_ig_for_range, [(df, y, start, end, top_k) for start, end in ranges])

    top_features = merge_heaps(results, top_k)

    top_features.sort(reverse=True, key=lambda x: x[0])

    print(f"\nTop {top_k} słów najbardziej informatywnych dla tagu '{selected_tag}':\n")
    for rank, (ig, idx) in enumerate(top_features, 1):
        print(f"{rank:2d}. Indeks słowa #{idx} - IG: {ig:.5f}")
