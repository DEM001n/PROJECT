import numpy as np
import matplotlib.pyplot as plt



def numerical_gradient(f, x, h=1e-5):
    grad = np.zeros_like(x)

    for i in range(len(x)):
        x_h1 = x.copy()
        x_h2 = x.copy()
        x_h1[i] += h
        x_h2[i] -= h
        grad[i] = (f(x_h1[0], x_h1[1]) - f(x_h2[0], x_h2[1])) / (2 * h)

    return grad

def gradient_descent(f, P0, alfa, epsilon=1e-5, max_iter=400):
    points = [P0]
    xk = np.array(P0, dtype=float)

    for k in range(max_iter):
        grad = numerical_gradient(f, xk)
        grad_norm = np.linalg.norm(grad)

        if grad_norm < epsilon:
            print(f'Warunek stopu osiągnięty w iteracji {k}')
            break

        ak = alfa / grad_norm
        xk = xk + ak * grad
        points.append(xk.copy())

    return xk, points

def gradient_descent_with_backtracking(f, P0, alfa, epsilon=1e-5, max_iter=400, beta=0.5, min_step=1e-8):
    points = [P0]
    xk = np.array(P0, dtype=float)

    for k in range(max_iter):
        grad = numerical_gradient(f, xk)
        grad_norm = np.linalg.norm(grad)

        if grad_norm < epsilon:
            print(f'Warunek stopu osiągnięty w iteracji {k}')
            break

        ak = alfa / grad_norm
        success = False

        while ak > min_step:
            x_new = xk + ak * grad
            if f(x_new[0], x_new[1]) >= f(xk[0], xk[1]):
                success = True
                break
            else:
                ak *= beta

        if not success:
            print(f'Nie znaleziono lepszego kroku w iteracji {k}')
            break

        xk = x_new
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
        (f1, (-10, 10), (-10, 10), (-10, 10), 1, 0.1, "Funkcja 1"),
        (f2, (-1, 0.4), (-1, 1), (-1, 1), 0.1, 0.02, "Funkcja 2"),
        (f3, (-20, 20), (-20, 20), (-10, 10), 1, 0.2, "Funkcja 3"),
        (f4, (-2, 2), (-1, 3), (-2, 3), 0.5, 0.1, "Funkcja 4"),
    ]

    for f, xrange, yrange, P0, alfa, step, title in tasks:
        result, points = gradient_descent(f, P0, alfa)
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
        (f1, (-10, 10), (-10, 10), (-10, 10), 1, 0.1, "Funkcja 1"),
        (f2, (-1, 0.4), (-1, 1), (-1, 1), 0.1, 0.02, "Funkcja 2"),
        (f3, (-20, 20), (-20, 20), (-10, 10), 1, 0.2, "Funkcja 3"),
        (f4, (-2, 2), (-1, 3), (-2, 3), 0.5, 0.1, "Funkcja 4"),
    ]

    for f, xrange, yrange, P0, alfa, step, title in tasks:
        result1, points1 = gradient_descent(f, P0, alfa)
        value1 = f(result1[0], result1[1])

        result2, points2 = gradient_descent_with_backtracking(f, P0, alfa)
        value2 = f(result2[0], result2[1])

        print(f"{title} - Zwykła metoda: maksimum = {result1}, wartość = {value1}, iteracje = {len(points1) - 1}")
        print(f"{title} - Backtracking: maksimum = {result2}, wartość = {value2}, iteracje = {len(points2) - 1}")

        fig = plt.figure(figsize=(18, 7))

        ax1 = fig.add_subplot(1, 2, 1, projection='3d')
        X = np.arange(xrange[0], xrange[1], step)
        Y = np.arange(yrange[0], yrange[1], step)
        X, Y = np.meshgrid(X, Y)
        Z = f(X, Y)
        ax1.plot_surface(X, Y, Z, cmap='viridis', alpha=0.7)
        points1 = np.array(points1)
        Z_points1 = f(points1[:, 0], points1[:, 1])
        ax1.plot(points1[:, 0], points1[:, 1], Z_points1, 'r.-', markersize=5)
        ax1.scatter(points1[-1, 0], points1[-1, 1], Z_points1[-1], color='black', s=50, label='Punkt końcowy')
        ax1.set_title(title + " - bez backtrackingu")
        ax1.set_xlabel('x')
        ax1.set_ylabel('y')
        ax1.set_zlabel('f(x,y)')
        ax1.legend()

        ax2 = fig.add_subplot(1, 2, 2, projection='3d')
        ax2.plot_surface(X, Y, Z, cmap='viridis', alpha=0.7)
        points2 = np.array(points2)
        Z_points2 = f(points2[:, 0], points2[:, 1])
        ax2.plot(points2[:, 0], points2[:, 1], Z_points2, 'r.-', markersize=5)
        ax2.scatter(points2[-1, 0], points2[-1, 1], Z_points2[-1], color='black', s=50, label='Punkt końcowy')
        ax2.set_title(title + " - z backtrackingiem")
        ax2.set_xlabel('x')
        ax2.set_ylabel('y')
        ax2.set_zlabel('f(x,y)')
        ax2.legend()

        plt.tight_layout()
        plt.show()



if __name__ == '__main__':
    #zad1()
    zad2()
