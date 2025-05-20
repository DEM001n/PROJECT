import numpy as np
import matplotlib.pyplot as plt
from scipy.integrate import quad



def f1(x):
    return np.abs(x) + 3

def f2(x):
    return np.exp(x**2)

def f3(x):
    return np.sin(5 * x)

def f4(x):
    return x**2



def monte_carlo_integration(f, a, b, n):
    x = np.random.uniform(a, b, n)
    y = f(x)
    integral = (b - a) * np.mean(y)
    return integral, x, y

def simpson_integration(f, a, b, h):
    n = int((b - a) / h)

    if n % 2 == 1:
        n += 1
        h = (b - a) / n

    x = np.linspace(a, b, n + 1)
    y = f(x)
    S = y[0] + y[-1] + 4 * sum(y[1:-1:2]) + 2 * sum(y[2:-2:2])
    return (h / 3) * S, x, y

def plot_simpson_parabolas(x, y, ax):

    def parabola_coefficients(x_points, y_points):
        A_matrix = np.array([
            [x_points[0] ** 2, x_points[0], 1],
            [x_points[1] ** 2, x_points[1], 1],
            [x_points[2] ** 2, x_points[2], 1]
        ])
        coeffs = np.linalg.solve(A_matrix, y_points)
        return coeffs

    for i in range(0, len(x)-2, 2):
        xi = x[i:i+3]
        yi = y[i:i+3]
        coeffs = parabola_coefficients(xi, yi)
        A, B, C = coeffs

        x_parabola = np.linspace(xi[0], xi[2], 100)
        y_parabola = A * x_parabola**2 + B * x_parabola + C
        ax.plot(x_parabola, y_parabola, 'r', lw=2, label='Parabola' if i==0 else "")

def plot_methods(f, a, b, step, func_name):
    fig, ax = plt.subplots(figsize=(10, 6))
    x_vals = np.linspace(a, b, 500)
    y_vals = f(x_vals)
    ax.plot(x_vals, y_vals, 'b-', label=f'{func_name}')

    n = int((b - a) / step) * 100
    mc_result, mc_x, mc_y = monte_carlo_integration(f, a, b, n)
    ax.scatter(mc_x, mc_y, color='green', alpha=0.3, label='Monte Carlo punkty')

    simpson_result, simp_x, simp_y = simpson_integration(f, a, b, step)
    ax.plot(simp_x, simp_y, 'ro')
    plot_simpson_parabolas(simp_x, simp_y, ax)

    ax.set_title(f"{func_name}, krok={step}\nMonte Carlo={mc_result:.4f}, Simpson={simpson_result:.4f}")
    ax.legend()
    ax.grid(True)
    plt.show()

def integral(f, a, b, steps, func_name):
    print(f"\nFunkcja: {func_name}, przedział: [{a}, {b}]")
    exact, _ = quad(f, a, b)
    print(f"Wartość dokładna (quad): {exact:.6f}")

    for step in steps:

        n = int((b - a) / step) * 100
        mc_result, _, _ = monte_carlo_integration(f, a, b, n)
        mc_error = abs(mc_result - exact)

        simpson_result, _, _ = simpson_integration(f, a, b, step)
        simpson_error = abs(simpson_result - exact)

        print(f"\n  Krok: {step}")
        print(f"    Monte Carlo: {mc_result:.6f}, błąd bezwzględny: {mc_error:.6f}")
        print(f"    Simpson:     {simpson_result:.6f}, błąd bezwzględny: {simpson_error:.6f}")

        # --- Zadanie2 ----
        plot_methods(f, a, b, step, func_name)



# --- Zadania1 ----
integral(f1, -4.5, 4.5, [3,1,0.5], "f(x) = |x| + 3")
integral(f2, 0, 1, [0.7,0.3,0.1], "f(x) = e^(x^2)")
integral(f3, -5, 4, [2,1,0.5], "f(x) = sin(5x)")
integral(f4, -3, 8, [2,1,0.5], "f(x) = x^2")


