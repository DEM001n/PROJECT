import numpy as np

np.set_printoptions(precision=2, suppress=True)

def simplex_method(A, b, c, z, maximize=True):
    m, n = A.shape
    A_mod = A.copy()
    slack_vars = []
    artificial_vars = []
    var_names = [f"x{i + 1}" for i in range(n)]

    for i in range(m):
        row_slack = np.zeros((m, 1))
        row_artif = np.zeros((m, 1))

        if z[i] == -1:  # <=
            row_slack[i] = 1
            A_mod = np.hstack([A_mod, row_slack])
            slack_vars.append(A_mod.shape[1] - 1)
            var_names.append(f"s{i + 1}")
        elif z[i] == 0:  # =
            row_artif[i] = 1
            A_mod = np.hstack([A_mod, row_artif])
            artificial_vars.append(A_mod.shape[1] - 1)
            var_names.append(f"a{i + 1}")
        elif z[i] == 1:  # >=
            row_slack[i] = -1
            row_artif[i] = 1
            A_mod = np.hstack([A_mod, row_slack, row_artif])
            slack_vars.append(A_mod.shape[1] - 2)
            artificial_vars.append(A_mod.shape[1] - 1)
            var_names.extend([f"s{i + 1}", f"a{i + 1}"])

    total_vars = A_mod.shape[1]

    c_ext = np.zeros(total_vars)
    c_ext[:len(c)] = c if maximize else -c
    for idx in artificial_vars:
        c_ext[idx] = -1e6

    tableau = np.hstack([A_mod, b.reshape(-1, 1)])

    basis = []
    for i in range(m):
        found = False
        for j in range(A_mod.shape[1]):
            col = A_mod[:, j]
            if np.allclose(col, np.eye(m)[:, i]):
                basis.append(j)
                found = True
                break
        if not found:
            raise Exception(f"Nie można znaleźć kolumny bazowej dla wiersza {i}.")

    cb = c_ext[basis]

    print("Iteracja 0:")
    print_tableau(cb, c_ext, tableau, basis, var_names)
    Z = np.dot(cb, tableau[:, -1])
    print(f"Wartość funkcji celu Z: {Z:.2f}")

    iter_count = 0
    while True:
        z_row = np.dot(cb, tableau[:, :-1])
        zj_cj = c_ext - z_row

        if np.all(zj_cj <= 1e-8):
            break

        pivot_col = np.argmax(zj_cj)
        ratios = np.divide(tableau[:, -1], tableau[:, pivot_col],
                           out=np.full(m, np.inf), where=tableau[:, pivot_col] > 1e-8)
        pivot_row = np.argmin(ratios)

        if np.isinf(ratios[pivot_row]):
            raise Exception("Rozwiązanie nieograniczone (unbounded).")

        tableau[pivot_row, :] /= tableau[pivot_row, pivot_col]

        for i in range(m):
            if i != pivot_row:
                tableau[i, :] -= tableau[i, pivot_col] * tableau[pivot_row, :]

        basis[pivot_row] = pivot_col
        cb = c_ext[basis]

        iter_count += 1
        print(f"\nIteracja {iter_count}:")
        print_tableau(cb, c_ext, tableau, basis, var_names)
        Z = np.dot(cb, tableau[:, -1])
        print(f"Wartość funkcji celu Z: {Z:.2f}")

    x = np.zeros(total_vars)
    for i in range(m):
        x[basis[i]] = tableau[i, -1]

    Z = np.dot(cb, tableau[:, -1])
    if not maximize:
        Z = -Z

    print("\nRozwiązanie optymalne:")
    for i in range(len(c)):
        print(f"{var_names[i]} = {x[i]:.4f}")
    print(f"Wartość funkcji celu (Z): {Z:.2f}")

def print_tableau(cb, c_ext, tableau, basis, var_names):
    m, n_plus1 = tableau.shape
    n = n_plus1 - 1
    print("Cb   Basis     Tabela")
    for i in range(m) :
        print(f"{cb[i]:5.1f}  {var_names[basis[i]]:<4} ", end='')
        print(" ".join(f"{val:8.2f}" for val in tableau[i]))
    z = np.dot(cb, tableau[:, :n])
    zj_cj = c_ext - z
    print("Zj - Cj:    ", end='')
    print(" ".join(f"{val:8.2f}" for val in zj_cj))

if __name__ == '__main__':

    '''
    a = np.array([
        [1, 1],    # x + y ≥ 4
        [2, 1],    # 2x + y ≥ 6
        [1, 0],    # x ≤ 5
        [0, 1]    # y ≤ 4
    ])

    b = np.array([4, 6, 5, 4])

    z = np.array([1, 1, -1, -1]) #(-1: ≤, 1: ≥)

    c = np.array([1, 2])
    '''

    a = np.array([
        [0.1, 0.2],
        [0.3, 0.1],
        [0.5, 0.0]
    ])

    b = np.array([300, 300, 400])

    z = np.array([-1, -1, -1])

    c = np.array([90, 55])

    simplex_method(a, b, c, z, maximize=True)

