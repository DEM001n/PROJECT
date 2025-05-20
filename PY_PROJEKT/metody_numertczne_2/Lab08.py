import numpy as np
import itertools
import matplotlib.pyplot as plt
from scipy.spatial import ConvexHull

def solve_linear_program(A, B, Z, F,maximize=True):
    points = []
    for i, j in itertools.combinations(range(len(A)), 2):
        A_ij = np.array([A[i], A[j]])
        B_ij = np.array([B[i], B[j]])
        try:
            point = np.linalg.solve(A_ij, B_ij)
            if np.all(np.isfinite(point)):
                points.append(point)
        except np.linalg.LinAlgError:
            continue

    extra_points = [
        np.array([0, 0]),
        np.array([0, B[1] / A[1][1]]) if A[1][1] != 0 else None,
        np.array([B[0] / A[0][0], 0]) if A[0][0] != 0 else None
    ]
    for pt in extra_points:
        if pt is not None:
            points.append(pt)

    unique_points = []
    seen = set()
    for pt in points:
        rounded = tuple(np.round(pt, 6))
        if rounded not in seen:
            seen.add(rounded)
            unique_points.append(np.array(rounded))
    points = unique_points

    feasible_points = []
    for pt in points:
        satisfies_all = True
        for i in range(len(A)):
            lhs = A[i] @ pt
            rhs = B[i]
            if Z[i] == -1 and lhs > rhs + 1e-8:
                satisfies_all = False
                break
            elif Z[i] == 1 and lhs < rhs - 1e-8:
                satisfies_all = False
                break
            elif Z[i] == 0 and abs(lhs - rhs) > 1e-8:
                satisfies_all = False
                break
        if satisfies_all:
            feasible_points.append(pt)

    values = [F @ pt for pt in feasible_points]
    values = np.array(values)

    if maximize:
        best_value = np.max(values)
        label = "Maksymalny"
    else:
        best_value = np.min(values)
        label = "Minimalny"

    epsilon = 1e-6
    optimal_points = [pt for pt, val in zip(feasible_points, values) if abs(val - best_value) < epsilon]

    print("Rozwiązania dopuszczalne i wartości funkcji celu:")
    for pt, val in zip(feasible_points, values):
        print(f"Punkt: ({pt[0]:.2f}, {pt[1]:.2f}) => Zysk: {val:.2f} zł")

    print(f"\n{label} zysk: {best_value:.2f} zł w punktach:")
    for pt in optimal_points:
        print(f"x1 = {pt[0]:.2f}, x2 = {pt[1]:.2f}")

    x_range = range(int(np.floor(min([pt[0] for pt in feasible_points])))-1, int(np.ceil(max([pt[0] for pt in feasible_points])))+1)
    y_range = range(int(np.floor(min([pt[1] for pt in feasible_points])))-1, int(np.ceil(max([pt[1] for pt in feasible_points])))+1)

    grid_optimal_points = []
    for xi in x_range:
        for yi in y_range:
            point = np.array([xi, yi])
            if not np.isclose(F @ point, best_value, atol=1e-6):
                continue
            satisfies_all = True
            for i in range(len(A)):
                lhs = A[i] @ point
                rhs = B[i]
                if Z[i] == -1 and lhs > rhs + 1e-8:
                    satisfies_all = False
                    break
                elif Z[i] == 1 and lhs < rhs - 1e-8:
                    satisfies_all = False
                    break
                elif Z[i] == 0 and abs(lhs - rhs) > 1e-8:
                    satisfies_all = False
                    break
            if satisfies_all:
                grid_optimal_points.append(point)

    print(f"\nWszystkie całkowite punkty optymalne:")
    for pt in grid_optimal_points:
        print(f"x1 = {pt[0]}, x2 = {pt[1]}")

    fig, ax = plt.subplots()
    x_vals = [pt[0] for pt in points]
    x_min = min(x_vals + [0]) - 1
    x_max = max(x_vals + [0]) + 1
    x = np.linspace(x_min, x_max, 400)

    for i in range(len(A)):
        if A[i][1] != 0:
            y = (B[i] - A[i][0] * x) / A[i][1]
            ax.plot(x, y, linestyle="-", color="black")
        elif A[i][0] != 0:
            x_line = B[i] / A[i][0]
            ax.axvline(x_line, linestyle="-", color="black")

    for pt in points:
        ax.plot(pt[0], pt[1], 'bo')

    px = [pt[0] for pt in feasible_points]
    py = [pt[1] for pt in feasible_points]
    ax.plot(px, py, 'ro', label='Wierzchołki obszaru dopuszczalnego')

    if len(feasible_points) >= 3:
        hull = ConvexHull(feasible_points)
        hull_pts = np.array(feasible_points)[hull.vertices]
        ax.fill(hull_pts[:, 0], hull_pts[:, 1], color='yellow', alpha=0.4, label="Obszar dopuszczalny")

    Gx, Gy = F[0], F[1]
    slope = -Gx / Gy if Gy != 0 else None

    for pt in feasible_points:
        if any(np.allclose(pt, opt_pt, atol=1e-6) for opt_pt in optimal_points):
            continue
        Px, Py = pt[0], pt[1]
        if slope is not None:
            y = slope * (x - Px) + Py
            ax.plot(x, y, linestyle='--', color='black')

    Px, Py = optimal_points[0]
    if slope is not None:
        y = slope * (x - Px) + Py
        ax.plot(x, y, 'g-', label='Warstwica funkcji celu (optymalna)')

    norm = np.linalg.norm(F)
    if norm > 0:
        gx_line = Gx / norm * x
        gy_line = Gy / norm * x
        ax.plot(gx_line, gy_line, color='magenta', linestyle='-', label='Gradient')

    for pt in optimal_points:
        ax.plot(pt[0], pt[1], 'go', label="Rozwiązanie optymalne")

    for pt in grid_optimal_points:
        ax.plot(pt[0], pt[1], 'gs', label='Całkowity punkt optymalny')

    ax.set_xlim(left=0)
    ax.set_ylim(bottom=0)
    ax.set_xlabel("x1")
    ax.set_ylabel("x2")
    ax.set_title("Graficzne rozwiązanie programu liniowego")
    ax.legend()
    plt.show()


if __name__ == '__main__':
    A = np.array([
        [1, 1],
        [1, 0],
        [0, 1],
        [1, 0],
        [0, 1],
        [1, 0],
        [0, 1]
    ])

    B = np.array([6, 2, 1, 5, 4, 0, 0])

    #(-1: ≤, 1: ≥)
    Z = np.array([-1, 1, 1, -1, -1, 1, 1])

    F = np.array([1, 1])

    solve_linear_program(A, B, Z, F)
