import numpy as np
import matplotlib.pyplot as plt



def zad1_2():
    def forward_difference(f, x, h):
        return (f(x + h) - f(x)) / h

    def backward_difference(f, x, h):
        return (f(x) - f(x - h)) / h

    def central_difference(f, x, h):
        return (f(x + h) - f(x - h)) / (2 * h)

    def example_function(x):
        return np.sin(x)

    def analytical_derivative(x):
        return np.cos(x)

    N = 100
    x_values = np.linspace(0, 2 * np.pi, N)
    h = 0.01

    forward_values = []
    backward_values = []
    central_values = []
    analytical_values = []
    forward_errors = 0.0
    backward_errors = 0.0
    central_errors = 0.0

    for x in x_values:
        forward = forward_difference(example_function, x, h)
        backward = backward_difference(example_function, x, h)
        central = central_difference(example_function, x, h)
        analytical = analytical_derivative(x)

        forward_values.append(forward)
        backward_values.append(backward)
        central_values.append(central)
        analytical_values.append(analytical)

        forward_errors += abs(forward - analytical)
        backward_errors += abs(backward - analytical)
        central_errors += abs(central - analytical)

    print(f"Sum of Forward Difference Errors: {forward_errors}")
    print(f"Sum of Backward Difference Errors: {backward_errors}")
    print(f"Sum of Central Difference Errors: {central_errors}")

    plt.plot(x_values, forward_values, label='Forward Difference', linestyle='dashed')
    plt.plot(x_values, backward_values, label='Backward Difference', linestyle='dotted')
    plt.plot(x_values, central_values, label='Central Difference', linestyle='dashdot')
    plt.plot(x_values, analytical_values, label='Analytical Derivative', linestyle='solid')
    plt.xlabel('x')
    plt.ylabel('Derivative Value')
    plt.legend()
    plt.title('Comparison of Numerical Differentiation Methods')
    plt.show()

def zad3():
    def forward_difference(f, x, h):
        return (f(x + h) - f(x)) / h

    def backward_difference(f, x, h):
        return (f(x) - f(x - h)) / h

    def central_difference(f, x, h):
        return (f(x + h) - f(x - h)) / (2 * h)

    def calculate_mae(errors, N):
        return np.sum(errors[2:N - 1]) / (N - 2)

    def example_function(x):
        return np.sin(x)

    def analytical_derivative(x):
        return np.cos(x)

    h = 0.01
    N_values = [10, 50, 100, 500,10000]

    mae_forward = []
    mae_backward = []
    mae_central = []

    for N in N_values:
        x_values = np.linspace(0, 2 * np.pi, N)

        forward_errors = []
        backward_errors = []
        central_errors = []

        for x in x_values:
            forward = forward_difference(example_function, x, h)
            backward = backward_difference(example_function, x, h)
            central = central_difference(example_function, x, h)
            analytical = analytical_derivative(x)

            forward_errors.append(abs(forward - analytical))
            backward_errors.append(abs(backward - analytical))
            central_errors.append(abs(central - analytical))

        mae_forward.append(calculate_mae(np.array(forward_errors), N))
        mae_backward.append(calculate_mae(np.array(backward_errors), N))
        mae_central.append(calculate_mae(np.array(central_errors), N))

    for i, N in enumerate(N_values):
        print(f'N = {N}:')
        print(f'  Forward Difference MAE: {mae_forward[i]}')
        print(f'  Backward Difference MAE: {mae_backward[i]}')
        print(f'  Central Difference MAE: {mae_central[i]}')

    '''
    Błąd praktycznie nie zmienił się w zależności od zmiany N (powoli zmniejsza się). 
    Metoda centralna wykazywa mniejszy błąd niż metody w przód i wstecz.
    '''

def zad4():

    def forward_difference(f, x, h):
        return (f(x + h) - f(x)) / h

    def backward_difference(f, x, h):
        return (f(x) - f(x - h)) / h

    def central_difference(f, x, h):
        return (f(x + h) - f(x - h)) / (2 * h)

    def example_function(x):
        return np.sin(x)

    def analytical_derivative(x):
        return np.cos(x)

    N = 100
    x_values = np.linspace(0, 2 * np.pi, N)
    h = 0.01

    forward_errors = []
    backward_errors = []
    central_errors = []

    for x in x_values:
        forward = forward_difference(example_function, x, h)
        backward = backward_difference(example_function, x, h)
        central = central_difference(example_function, x, h)
        analytical = analytical_derivative(x)

        forward_errors.append(abs(forward - analytical))
        backward_errors.append(abs(backward - analytical))
        central_errors.append(abs(central - analytical))

    plt.figure(figsize=(10, 6))

    plt.plot(x_values, forward_errors, label='Forward Difference Error', linestyle='dashed')
    plt.plot(x_values, backward_errors, label='Backward Difference Error', linestyle='dotted')
    plt.plot(x_values, central_errors, label='Central Difference Error', linestyle='dashdot')

    plt.xlabel('x')
    plt.ylabel('Absolute Error')
    plt.legend()
    plt.title('Error Comparison for Different Differentiation Methods')
    plt.show()

    '''
    Metoda centralna jest najdokładniejsza - metoda centralna daje najdokładniejsze wyniki, a jej błąd pozostaje prawie stały w całym przedziale.
    
    Metody w przód i wstecz mają wyższy błąd - ich dokładność spada w miarę oddalania się od środka przedziału, 
    a błąd na brzegach rośnie.
    Również można zaobserwować zmniejszenie błędu w miarę zbliżania się do punktów załamania wykresu funkcji sinusoidalnej 
    oraz jego wzrost w miarę oddalania się od tych punktów – największy błąd 
    występuje w miejscu, które jest równo oddalone od punktów załamania funkcji sinusoidalnej.
    '''

def zad5():
    def forward_difference(f, x, h):
        return (f(x + h) - f(x)) / h

    def backward_difference(f, x, h):
        return (f(x) - f(x - h)) / h

    def central_difference(f, x, h):
        return (f(x + h) - f(x - h)) / (2 * h)

    def example_function(x):
        return np.sin(x)

    def analytical_derivative(x):
        return np.cos(x)

    x0 = np.pi / 4

    h_values = [0.1, 0.01, 0.001, 0.0001,0.00001,0.000001,0.0000001]

    forward_errors = []
    backward_errors = []
    central_errors = []

    for h in h_values:
        forward = forward_difference(example_function, x0, h)
        backward = backward_difference(example_function, x0, h)
        central = central_difference(example_function, x0, h)
        analytical = analytical_derivative(x0)

        forward_errors.append(abs(forward - analytical))
        backward_errors.append(abs(backward - analytical))
        central_errors.append(abs(central - analytical))

    for i, h in enumerate(h_values):
        print(f"h = {h:.4f}:")
        print(f"  Forward Difference Error: {forward_errors[i]:.16f}")
        print(f"  Backward Difference Error: {backward_errors[i]:.16f}")
        print(f"  Central Difference Error: {central_errors[i]:.16f}")
        print()

    '''
    Metoda centralna jest najbardziej precyzyjna przy obliczaniu pochodnej, zwłaszcza przy mniejszych krokach h, 
    ponieważ jej błąd jest proporcjonalny do O(h^2)
    co oznacza, że zmniejszając h, błąd maleje szybciej w porównaniu do innych metod.
    
    Metody różnicy w przód i wstecz są mniej precyzyjne, ponieważ ich błąd jest proporcjonalny do O(h)
    więc chociaż maleją z mniejszym krokiem, nie poprawiają się tak znacząco jak metoda centralna.
    
    po pewnym zmniejszeniu h < 0.00001, wyniki zaczynają osiągać granice precyzji reprezentacji liczb zmiennoprzecinkowych w komputerze. 
    Dalsze zmniejszanie h prowadzi do utraty dokładności obliczeń, a błędy zaokrągleń zaczynają dominować.  
    '''



if __name__ == "__main__":
    zad1_2()
    zad3()
    zad4()
    zad5()

