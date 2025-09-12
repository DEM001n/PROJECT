import math
import matplotlib.pyplot as plt
import numpy as np



class AsymmetricInterval:
    def __init__(self, a: float, b: float, c: float):
        assert a <= c <= b, "Wartość oczekiwana musi znajdować się w przedziale"
        self.a = a
        self.b = b
        self.c = c

    def __str__(self):
        return f"[{self.a}, {self.b}].{self.c}"

    def alpha(self):
        return (self.b - self.c) / ((self.b - self.a) * (self.c - self.a))

    def beta(self):
        return (self.c - self.a) / ((self.b - self.a) * (self.b - self.c))

    def asymmetry(self):
        if self.a == self.b:
            return 0
        return (self.a + self.b - 2 * self.c) / (self.b - self.a)

    def add(self, other):
        return AsymmetricInterval(
            self.a + other.a,
            self.b + other.b,
            self.c + other.c
        )

    def subtract(self, other):
        return AsymmetricInterval(
            self.a - other.b,
            self.b - other.a,
            self.c - other.c
        )

    def multiply(self, other):
        products = [
            self.a * other.a,
            self.a * other.b,
            self.b * other.a,
            self.b * other.b
        ]
        return AsymmetricInterval(
            min(products),
            max(products),
            self.c * other.c
        )

    def divide(self, other):
        assert not (other.a <= 0 <= other.b), "Dzielenie przez przedział zawierający 0!"
        quotients = [
            self.a / other.a,
            self.a / other.b,
            self.b / other.a,
            self.b / other.b
        ]
        if other.a == other.b:
            c3 = self.c / other.c
        else:
            alpha = other.alpha()
            beta = other.beta()
            c3 = self.c * (alpha * math.log(other.c / other.a) + beta * math.log(other.b / other.c))
        return AsymmetricInterval(
            min(quotients),
            max(quotients),
            c3
        )

    def sqrt(self):
        assert self.a >= 0, "Pierwiastek tylko dla wartości >= 0"
        sqrt_a = math.sqrt(self.a)
        sqrt_b = math.sqrt(self.b)
        if self.a == self.b:
            sqrt_c = math.sqrt(self.c)
        else:
            alpha = self.alpha()
            beta = self.beta()
            sqrt_c = (2 / 3) * (alpha * (self.c ** 1.5 - self.a ** 1.5) + beta * (self.b ** 1.5 - self.c ** 1.5))
        return AsymmetricInterval(sqrt_a, sqrt_b, sqrt_c)

    def pow(self, n):
        assert self.a >= 0, "Potęgowanie tylko dla wartości >= 0"
        pow_a = self.a ** n
        pow_b = self.b ** n
        if self.a == self.b:
            pow_c = self.c ** n
        else:
            alpha = self.alpha()
            beta = self.beta()
            pow_c = 1 / (n + 1) * (
                alpha * (self.c ** (n + 1) - self.a ** (n + 1)) +
                beta * (self.b ** (n + 1) - self.c ** (n + 1))
            )
        return AsymmetricInterval(pow_a, pow_b, pow_c)

    def plot_pdf(self, ax=None):
        alpha = self.alpha()
        beta = self.beta()
        x = np.linspace(self.a - 1, self.b + 1, 500)
        y = np.piecewise(x,
                         [x < self.a,
                          (x >= self.a) & (x <= self.c),
                          (x > self.c) & (x <= self.b),
                          x > self.b],
                         [0,
                          lambda x: alpha,
                          lambda x: beta,
                          0])
        if ax is None:
            fig, ax = plt.subplots()
        ax.plot(x, y, label=str(self))
        ax.set_title("Rozkład gęstości prawdopodobieństwa (PDF)")
        ax.set_xlabel("x")
        ax.set_ylabel("f(x)")
        ax.legend()

class Interval:
    def __init__(self, a, b):
        assert a <= b, "Lewa granica musi być mniejsza lub równa prawej"
        self.a = a
        self.b = b

    def __str__(self):
        return f"[{self.a}, {self.b}]"

    def midpoint(self):
        return (self.a + self.b) / 2

    def add(self, other):
        return Interval(self.a + other.a, self.b + other.b)

    def subtract(self, other):
        return Interval(self.a - other.b, self.b - other.a)

    def multiply(self, other):
        products = [
            self.a * other.a,
            self.a * other.b,
            self.b * other.a,
            self.b * other.b
        ]
        return Interval(min(products), max(products))

    def divide(self, other):
        assert not (other.a <= 0 <= other.b), "Nie można dzielić przez przedział zawierający 0"
        quotients = [
            self.a / other.a,
            self.a / other.b,
            self.b / other.a,
            self.b / other.b
        ]
        return Interval(min(quotients), max(quotients))

    def inverse(self):
        assert not (self.a <= 0 <= self.b), "Odwrotność nie istnieje dla przedziału zawierającego 0"
        return Interval(1 / self.b, 1 / self.a)

    def sqrt(self):
        assert self.a >= 0, "Pierwiastek tylko dla nieujemnych"
        return Interval(math.sqrt(self.a), math.sqrt(self.b))



if __name__ =='__main__':
    X1 = Interval(500.0, 730.0)
    Y1 = Interval(30.0, 43.0)

    Y_div_X_CIN = Y1.divide(X1)
    TAC_CIN = Y_div_X_CIN.midpoint() * 100

    #-------------------------------------------------------

    X2 = AsymmetricInterval(500.0, 730.0, 615.0)
    Y2 = AsymmetricInterval(30.0, 43.0, 36.5)

    Y_div_X_AIN = Y2.divide(X2)
    TAC_AIN = Y_div_X_AIN.c * 100

    #-------------------------------------------------------

    ref = 5.955
    print( f"roznicz CIN: {abs(ref - TAC_CIN)}")
    print( f"roznicz AIN: {abs(ref - TAC_AIN)}")
