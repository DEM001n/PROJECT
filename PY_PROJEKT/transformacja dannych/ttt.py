import pandas as pd
import math

def freq(x, prob=True):
    freq_dict = {}
    for val in x:
        freq_dict[val] = freq_dict.get(val, 0) + 1
    xi = list(freq_dict.keys())
    ni = list(freq_dict.values())
    if prob:
        total = sum(ni)
        ni = [count / total for count in ni]
    return xi, ni

def entropy(x):
    _, probs = freq(x, prob=True)
    h = -sum(p * math.log2(p) for p in probs if p > 0)
    return h

def infogain(x, y):
    h_y = entropy(y)
    cond_entropy = 0
    xi, ni = freq(x, prob=True)
    for x_val, p_x in zip(xi, ni):
        y_given_x = [y[i] for i in range(len(x)) if x[i] == x_val]
        h_y_given_x = entropy(y_given_x)
        cond_entropy += p_x * h_y_given_x
    return h_y - cond_entropy

df = pd.read_csv("zoo.csv")
columns = [col for col in df.columns if col != "animal"]

info_gains = []
checked_pairs = set()

for col1 in columns:
    for col2 in columns:
        if col1 == col2:
            continue
        pair = tuple(sorted([col1, col2]))
        if pair in checked_pairs:
            continue
        checked_pairs.add(pair)

        ig1 = infogain(df[col1], df[col2])
        ig2 = infogain(df[col2], df[col1])

        info_gains.append((col1, col2, max(ig1, ig2)))

info_gains.sort(key=lambda x: x[2], reverse=True)

print("Unikalne pary cech posortowane wg przyrostu informacji (największy IG):")
for col1, col2, ig in info_gains:
    print(f"Infogain({col1} <-> {col2}) = {ig:.4f}")
