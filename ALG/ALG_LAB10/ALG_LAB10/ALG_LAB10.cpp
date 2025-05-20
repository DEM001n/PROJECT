#include <complex>
#include <iostream>
#include <vector>
#include <cmath> 

using namespace std;

const double PI = acos(-1);
const double MINERR = 0.000001;

template <typename T>
void norm(vector<complex<T>>& a, double e)
{
    for (size_t i = 0; i < a.size(); ++i) {
        if (abs(a[i].real()) < e)
            a[i] = complex<T>(0, a[i].imag());

        if (abs(a[i].imag()) < e)
            a[i] = complex<T>(a[i].real(), 0);
    }
}

template <typename T>
vector<complex<T>> DFT(const vector<complex<T>>& f, int N) {
    vector<complex<T>> c(N, complex<T>(0, 0));

    for (int i = 0; i < N; ++i)
        for (int j = 0; j < N; ++j) {
            double E = -2 * PI * i * j / N;
            c[i] += f[j] * exp(complex<T>(0, E));
        }

    norm<T>(c, MINERR);

    return c;
}

template <typename T>
vector<complex<T>> FFT(const vector<complex<T>>& f, size_t N) {
    if (N == 1) {
        return { f[0] };
    }

    vector<complex<T>> even(N / 2), odd(N / 2);
    for (size_t i = 0; i < N / 2; ++i) {
        even[i] = f[2 * i];
        odd[i] = f[2 * i + 1];
    }

    vector<complex<T>> E = FFT(even, N / 2);
    vector<complex<T>> O = FFT(odd, N / 2);

    vector<complex<T>> c(N);

    for (size_t k = 0; k < N / 2; ++k) {
        complex<T> Oe = exp(complex<T>(0, -2.0 * PI * k / N)) * O[k];

        c[k] = E[k] + Oe;
        c[k + N / 2] = E[k] - Oe;
    }

    norm<T>(c, MINERR);

    return c;
}

template <typename T>
T clcError(const vector<complex<T>>& dft, const vector<complex<T>>& fft) {
    size_t N = dft.size();
    T err = 0;

    for (size_t k = 0; k < N; ++k)
        err += abs(dft[k] - fft[k]);

    return err / (T)N;
}

template <typename T>
string strComplex(const std::complex<T>& value) {
    std::ostringstream oss;
    oss << value.real();

    if (value.imag() < 0)
        oss << value.imag() << "i";
    else
        oss << "+" << value.imag() << "i";

    return oss.str();
}


int main()
{
    vector<complex<double>> a = { {1,0}, { -2,0 }, { -3,0 }, { 5 ,0 } };

    vector<complex<double>> res1 = DFT(a, a.size());

    for (const auto& val : res1)
        cout << strComplex(val) << "\n";

    cout << "===========================\n";

    vector<complex<double>> b = { {1,0}, { -2,0 }, { -3,0 }, { 5,0 },{1,0}, { -2,0 }, { -3,0 }, { 5,0 } };

    vector<complex<double>> res2 = FFT(b, b.size());

    for (const auto& val : res2)
        cout << strComplex(val) << "\n";

    cout << "===========================\n";

    double err = clcError(res1, res2);

    cout << (err > MINERR ? err : 0);

    return 0;
}