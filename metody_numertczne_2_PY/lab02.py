import math
import numpy as np
import matplotlib.pyplot as plt



def numerical_derivative(f, x, h=1e-5):
    return (f(x + h) - f(x - h)) / (2 * h)

def numerical_second_derivative(f, x, h=1e-5):
    return (f(x + h) - 2 * f(x) + f(x - h)) / (h ** 2)



def funkcja1(x):
    return math.exp(x - 1) - 2

def funkcja2(x):
    return 2 ** (-x) + math.exp(x) + 2 * math.cos(x) - 6

def funkcja3(x):
    return math.log(x + 2 / 3)

def funkcja4(x):
    return x ** 3 - 2 * x - 5



def zad1():

    def metoda_stycznych(f, a, b, tol=1e-5, ftol=1e-4, max_iter=100):
        if f(a) * f(b) > 0:
            return None, 0

        fpp_a = numerical_second_derivative(f, a)

        if f(a) * fpp_a > 0:
            x = a
        else:
            x = b

        iteration = 0

        while iteration < max_iter:
            fx = f(x)
            dfx = numerical_derivative(f, x)

            if dfx == 0:
                return None, iteration

            x_new = x - fx / dfx

            if abs(f(x_new)) < ftol or abs(x_new - x) <= tol:
                return x_new, iteration + 1

            x = x_new
            iteration += 1

        return x, iteration

    def metoda_siecznych(f, a, b, tol=1e-5, ftol=1e-4, max_iter=100):
        if f(a) * f(b) > 0:
            return None, 0

        fpp_a = numerical_second_derivative(f, a)

        if f(a) * fpp_a > 0:
            x0 = a
            x1 = b
        else:
            x0 = b
            x1 = a

        iteration = 0

        while iteration < max_iter:
            f_x0 = f(x0)
            f_x1 = f(x1)

            if f_x1 - f_x0 == 0:
                return None, iteration

            x2 = x1 - f_x1 * ((x1 - x0) / (f_x1 - f_x0))

            if abs(f(x2)) < ftol or abs(x2 - x1) <= tol:
                return x2, iteration + 1

            x0, x1 = x1, x2
            iteration += 1

        return x2, iteration

    def plot_function(f, a, b, styczne_result, sieczne_result, title):
        x = np.linspace(a, b, 1000)
        y = [f(xi) for xi in x]

        plt.plot(x, y, label="f(x)")
        plt.axhline(0, color='black', linewidth=0.5)
        plt.scatter(styczne_result[0], f(styczne_result[0]), color='red', label='Styczne', marker='o')
        plt.scatter(sieczne_result[0], f(sieczne_result[0]), color='green', label='Sieczne', marker='x')
        plt.legend()
        plt.xlabel("x")
        plt.ylabel("f(x)")
        plt.title(title)
        plt.show()

    testy = [
        (funkcja1, -1, 2, "f(x) = e^(x-1) - 2"),
        (funkcja2, 1, 3, "f(x) = 2^(-x) + e^x + 2cos(x) - 6"),
        (funkcja3, -0.5, 2, "f(x) = log(x + 2/3)"),
        (funkcja4, 0, 3, "f(x) = x^3 - 2x - 5")
    ]

    for f, a, b, opis in testy:
        print(f"\n{opis} na przedziale [{a}, {b}]")
        styczne_result = metoda_stycznych(f, a, b)
        sieczne_result = metoda_siecznych(f, a, b)
        print(f"Metoda stycznych: x = {styczne_result[0]:.6f}, Iteracje = {styczne_result[1]}")
        print(f"Metoda siecznych: x = {sieczne_result[0]:.6f}, Iteracje = {sieczne_result[1]}")
        plot_function(f, a, b, styczne_result, sieczne_result, opis)



def zad2():

    def metoda_siecznych(f, a, b, tol=1e-5, ftol=1e-4, max_iter=100):
        if f(a) * f(b) > 0:
            print(f"Brak pierwiastka na przedziale [{a}, {b}].")
            return None, 0, []

        fpp_a = numerical_second_derivative(f, a)

        if f(a) * fpp_a > 0:
            x0 = a
            x1 = b
        else:
            x0 = b
            x1 = a

        iteration = 0
        steps = []

        while iteration < max_iter:
            f_x0 = f(x0)
            f_x1 = f(x1)

            if f_x1 - f_x0 == 0:
                return None, iteration, steps

            x2 = x1 - f_x1 * ((x1 - x0) / (f_x1 - f_x0))

            steps.append((x0, x1))

            if abs(f(x2)) < ftol or abs(x2 - x1) <= tol:
                return x2, iteration + 1, steps

            x0, x1 = x1, x2
            iteration += 1

        return x2, iteration, steps

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
            plt.plot([a_i, b_i], [f(a_i), f(b_i)], color='red')

            plt.legend()
            plt.xlabel("x")
            plt.ylabel("f(x)")
            plt.title("Wizualizacja metody siecznych")
            plt.pause(0.5)

        plt.show()

    testy = [
        (funkcja1, -1, 2, "f(x) = e^(x-1) - 2"),
        (funkcja2, 1, 3, "f(x) = 2^(-x) + e^x + 2cos(x) - 6"),
        (funkcja3, -0.5, 2, "f(x) = log(x + 2/3)"),
        (funkcja4, 0, 3, "f(x) = x^3 - 2x - 5")
    ]

    for f, a, b, opis in testy:
        print(f"\nTest: {opis}")
        try:
            wynik, iteracje, steps = metoda_siecznych(f, a, b)
            print(f"Znaleziony pierwiastek: x = {wynik:.6f}, liczba iteracji = {iteracje}")
            visualization(f, a, b, steps)
        except Exception as e:
            print(f"Błąd dla funkcji {opis}: {e}")

if __name__ == "__main__":
    #zad1()
    zad2()
