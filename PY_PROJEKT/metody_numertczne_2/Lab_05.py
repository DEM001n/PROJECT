import time
import matplotlib.pyplot as plt
import numpy as np
from scipy.optimize import minimize_scalar



def derivative(f, x, h=1e-5):
    return (f(x + h) - f(x - h)) / (2 * h)

def derivative_2(f, x, h=1e-5):
    return (f(x + h) - 2*f(x) + f(x - h)) / (h ** 2)

def bisection_max(f, a, b, epsilon=1e-5):
    start_time = time.time()
    iterations = 0

    if derivative(f, a) <= 0 or derivative(f, b) >= 0:
        result = (a, iterations) if f(a) > f(b) else (b, iterations)
        elapsed_time = time.time() - start_time
        return (result[0], result[1], elapsed_time)

    while (b - a) > epsilon:
        iterations += 1
        L = b - a
        x1 = a + 0.25 * L
        x2 = b - 0.25 * L

        if f(x1) < f(x2):
            a = x1
        else:
            b = x2

        xm = (a + b) / 2
        if abs(derivative(f, xm)) < epsilon:
            break

    elapsed_time = time.time() - start_time
    return ((a + b) / 2, iterations, elapsed_time)

def powell_quadratic_approx(f, a, b, epsilon=1e-5):
    start_time = time.time()
    iterations = 0
    x1 = a
    x3 = b
    x2 = (x1 + x3) / 2
    delta = x2 - x1

    while (x3 - x1) > epsilon:
        iterations += 1
        f1, f2, f3 = f(x1), f(x2), f(x3)
        N = f3 - f1
        D = f1 - 2*f2 + f3
        if D == 0:
            break

        x_new = x2 - 0.5 * delta * (N / D)

        x1 = x_new - delta / 2
        x2 = x_new
        x3 = x_new + delta / 2
        delta /= 2

        if abs(derivative(f, x2)) < epsilon:
            break

    elapsed_time = time.time() - start_time
    return (x2, iterations, elapsed_time)



def validate_maximum(f, a, b, x, epsilon=1e-4):
    d1 = derivative(f, x)
    d2 = derivative_2(f, x)

    if not (a <= x <= b):
        print(f"nie leży w przedziale [{a}, {b}]")
    if abs(d1) >= epsilon:
        print(f"f'(x) = nie jest bliskie zeru")
    if d2 >= 0:
        print(f'f"(x) = nie jest mniejsze od 0')

def compare_methods(f, a, b, epsilon=1e-4):
    print()

    max_bisection, iter_bis, time_bis = bisection_max(f, a, b, epsilon)
    max_powell, iter_pow, time_pow = powell_quadratic_approx(f, a, b, epsilon)

    result = minimize_scalar(lambda x: -f(x), bounds=(a, b), method='bounded')
    max_true = result.x

    print(f"Bisection: max = {max_bisection:.6f}, iteracje = {iter_bis}, czas = {time_bis:.11f} s, błąd = {abs(max_bisection - max_true):.2e}")
    validate_maximum(f,a,b,max_bisection)

    print(f"Powell   : max = {max_powell:.6f}, iteracje = {iter_pow}, czas = {time_pow:.11f} s, błąd = {abs(max_powell - max_true):.2e}")
    validate_maximum(f,a,b,max_powell)

    print(f"SciPy    : max = {max_true:.6f}")

    return (max_bisection, max_powell, max_true)

def plot_function_with_extrema(f, a, b, max_bis, max_pow, max_true):
    x = np.linspace(a, b, 1000)
    y = [f(val) for val in x]

    plt.figure(figsize=(8, 5))
    plt.plot(x, y, color='black')

    plt.plot(max_bis, f(max_bis), 'ro', label='Bisection Max')
    plt.plot(max_pow, f(max_pow), 'bs', label='Powell Max')
    plt.plot(max_true, f(max_true), 'g^', label='SciPy Max')

    plt.title(f'Maksimum funkcji')
    plt.xlabel('x')
    plt.ylabel('f(x)')
    plt.grid(True)
    plt.legend()
    plt.show()



def f1(x): return -x**2 + x
def f2(x): return -1/3 * x**2 - 13/7 * x + 11
def f3(x): return x**4 - 12*x**3 + x + 4
def f4(x): return -x**3 + 3*x**2 - 3*x



if __name__ == '__main__':
    test_functions = [
        (f1, 0.25, 0.75),
        (f2, -10, 10),
        (f3, -2, 2),
        (f4, -2, -1)
    ]

    for func, a, b in test_functions:
        max_bis, max_pow, max_true = compare_methods(func, a, b)
        plot_function_with_extrema(func, a, b, max_bis, max_pow, max_true)
