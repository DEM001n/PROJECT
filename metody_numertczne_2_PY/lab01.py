import math
import numpy as np
import matplotlib.pyplot as plt


def f1(x):
    return math.cos(x)

def f2(x):
    return 2 ** (-x) + math.exp(x) + 2 * math.cos(x) - 6

def f3(x):
    return (x + 2) ** 5



def zad_1():
    def bisection_method(f, a, b, tol=1e-5, ftol=1e-4, max_iter=100):
        if f(a) * f(b) >= 0:
            print("Błąd: Funkcja musi zmieniać znak w przedziale [a, b]")
            return None, 0

        iteration = 0
        while (b - a) / 2 > tol and iteration < max_iter:
            x0 = (a + b) / 2
            if abs(f(x0)) <= ftol:
                return x0, iteration

            if f(a) * f(x0) < 0:
                b = x0
            else:
                a = x0

            iteration += 1

        return (a + b) / 2, iteration

    def regula_falsi(f, a, b, tol=1e-5, ftol=1e-4, max_iter=100):
        if f(a) * f(b) >= 0:
            print("Błąd: Funkcja musi zmieniać znak w przedziale [a, b]")
            return None, 0

        def second_derivative(f, x, h=1e-5):
            return (f(x + h) - 2 * f(x) + f(x - h)) / h ** 2

        if f(a) * second_derivative(f, a) > 0:
            x_s, x_0 = a, b
        else:
            x_s, x_0 = b, a

        iteration = 0
        while abs(f(x_0)) > ftol and abs(x_0 - x_s) > tol and iteration < max_iter:
            x_new = x_s - (f(x_s) * (x_0 - x_s)) / (f(x_0) - f(x_s))
            if abs(f(x_new)) <= ftol or abs(x_new - x_0) <= tol:
                return x_new, iteration
            x_s, x_0 = x_new, x_s
            iteration += 1

        return x_new, iteration

    def plot_function(f, a, b, bisec_result, falsi_result):
        x = np.linspace(a, b, 1000)
        y = [f(xi) for xi in x]

        plt.plot(x, y, label="f(x)")
        plt.axhline(0, color='black', linewidth=0.5)
        plt.scatter(bisec_result[0], f(bisec_result[0]), color='red', label='Bisekcja', marker='o')
        plt.scatter(falsi_result[0], f(falsi_result[0]), color='blue', label='Falsi', marker='x')
        plt.legend()
        plt.xlabel("x")
        plt.ylabel("f(x)")
        plt.title("Porównanie metod")
        plt.show()

    functions = [(f1, 0, 2), (f2, 1, 3), (f3, -3, 0)]

    for f, a, b in functions:
        bisec_result = bisection_method(f, a, b)
        falsi_result = regula_falsi(f, a, b)
        print(f"Funkcja: {f.__name__}")
        print(f"Bisekcja: x = {bisec_result[0]}, iteracje = {bisec_result[1]}")
        print(f"Regula falsi: x = {falsi_result[0]}, iteracje = {falsi_result[1]}")
        plot_function(f, a, b, bisec_result, falsi_result)



def zad_2():
    def bisection_method(f, a, b, tol=1e-5, ftol=1e-4, max_iter=100):
        if f(a) * f(b) >= 0:
            print("Błąd: Funkcja musi zmieniać znak w przedziale [a, b]")
            return None, 0, []

        iteration = 0
        intervals = [(a, b)]
        while (b - a) / 2 > tol and iteration < max_iter:
            x0 = (a + b) / 2
            if abs(f(x0)) <= ftol:
                return x0, iteration, intervals

            if f(a) * f(x0) < 0:
                b = x0
            else:
                a = x0

            intervals.append((a, b))
            iteration += 1

        return (a + b) / 2, iteration, intervals

    def visualization(f, a, b, intervals):
        x = np.linspace(a, b, 1000)
        y = [f(xi) for xi in x]

        for (a_i, b_i) in intervals:
            plt.clf()

            plt.plot(x, y, color='blue', label="f(x)")
            plt.axhline(0, color='black', linewidth=0.5)

            x_interval = np.linspace(a_i, b_i, 1000)
            y_interval = [f(xi) for xi in x_interval]
            plt.plot(x_interval, y_interval, color='cyan')

            plt.scatter([a_i, b_i], [f(a_i), f(b_i)], color='green')
            plt.plot([a_i, b_i], [0, 0], color='red')

            plt.legend()
            plt.xlabel("x")
            plt.ylabel("f(x)")
            plt.title("Wizualizacja metody bisekcji")
            plt.pause(0.5)

        plt.show()

    functions = [(f1, 0, 2), (f2, 1, 3), (f3, -3, 0)]

    for f, a, b in functions:
        bisec_result, iter_count, intervals = bisection_method(f, a, b)
        print(f"Funkcja: {f.__name__}")
        print(f"Bisekcja: x = {bisec_result}, iteracje = {iter_count}")
        visualization(f, a, b, intervals)



if __name__ == "__main__":
    zad_1()
    #zad_2()

