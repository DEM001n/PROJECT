import numpy as np
import matplotlib.pyplot as plt
from scipy.optimize import minimize_scalar



def numerical_gradient(f, x, h=1e-5):
    grad = np.zeros_like(x)
    for i in range(len(x)):
        x_h1 = x.copy()
        x_h2 = x.copy()
        x_h1[i] += h
        x_h2[i] -= h
        grad[i] = (f(x_h1[0], x_h1[1]) - f(x_h2[0], x_h2[1])) / (2 * h)
    return grad

def gradient_descent(f, P0, epsilon=1e-5, max_iter=400):
    points = [np.array(P0, dtype=float)]
    xk = np.array(P0, dtype=float)

    for k in range(max_iter):
        grad = numerical_gradient(f, xk)
        grad_norm = np.linalg.norm(grad)

        if grad_norm < epsilon:
            print(f'Warunek stopu osiągnięty w iteracji {k}')
            break

        def phi(a):
            point = xk + a * grad
            return -f(point[0], point[1])

        res = minimize_scalar(phi, bounds=(0, 1), method='bounded', options={'xatol': 1e-6, 'maxiter': 100})
        ak = res.x

        xk = xk + ak * grad
        points.append(xk.copy())

    return xk, points



def f1(x, y):
    return -x ** 2 - y ** 2

def f2(x, y):
    return -3 * x ** 4 + (2 / 3) * y ** 3 - 2 * x ** 2 * y + 2 * x ** 2 - y ** 2

def f3(x, y):
    return -5 * (x - 1) ** 2 - 10 * (y - 3) ** 2

def f4(x, y):
    return -100 * (y - x ** 2) ** 2 - (1 - x) ** 2



def zad1():
    tasks = [
        (f1, (-10, 10), (-10, 10), (-10, 10), 0.1, "Funkcja 1"),
        (f2, (-1, 0.4), (-1, 1), (-1, 1), 0.02, "Funkcja 2"),
        (f3, (-20, 20), (-20, 20), (-10, 10), 0.2, "Funkcja 3"),
        (f4, (-2, 2), (-1, 3), (-2, 3), 0.1, "Funkcja 4"),
    ]

    for f, xrange, yrange, P0, step, title in tasks:
        result, points = gradient_descent(f, P0)
        print(f"{title} - znalezione maksimum: {result}, wartość funkcji: {f(result[0], result[1])}")

        fig = plt.figure(figsize=(10, 7))
        ax = fig.add_subplot(111, projection='3d')

        X = np.arange(xrange[0], xrange[1], step)
        Y = np.arange(yrange[0], yrange[1], step)
        X, Y = np.meshgrid(X, Y)
        Z = f(X, Y)

        ax.plot_surface(X, Y, Z, cmap='viridis', alpha=0.7)

        if points:
            points = np.array(points)
            Z_points = f(points[:, 0], points[:, 1])
            ax.plot(points[:, 0], points[:, 1], Z_points, 'r.-', markersize=5)
            ax.scatter(points[-1, 0], points[-1, 1], Z_points[-1], color='black', s=50, label='Punkt końcowy')
            ax.legend()

        ax.set_title(title)
        ax.set_xlabel('x')
        ax.set_ylabel('y')
        ax.set_zlabel('f(x,y)')
        plt.show()

def zad2():
    tasks = [
        (f1, (-10, 10), (-10, 10), (-10, 10), 0.1, "Funkcja 1"),
        (f2, (-1, 0.4), (-1, 1), (-1, 1), 0.02, "Funkcja 2"),
        (f3, (-20, 20), (-20, 20), (-10, 10), 0.2, "Funkcja 3"),
        (f4, (-2, 2), (-1, 3), (-2, 3), 0.1, "Funkcja 4"),
    ]

    for f, xrange, yrange, P0, step, title in tasks:
        result, points = gradient_descent(f, P0)
        print(f"{title} - znalezione maksimum: {result}, wartość funkcji: {f(result[0], result[1])}")

        if points:
            points = np.array(points)

            iterations = np.arange(len(points))
            xs = points[:, 0]
            ys = points[:, 1]

            plt.figure(figsize=(8, 5))
            plt.plot(iterations, xs, label='x', marker='o')
            plt.plot(iterations, ys, label='y', marker='s')
            plt.xlabel('Iteracja')
            plt.ylabel('Wartości zmiennych')
            plt.title(f'Zmiana wartości x i y w czasie - {title}')
            plt.legend()
            plt.grid(True)
            plt.show()



if __name__ == '__main__':
    zad1()
    zad2()
