#include <iostream>
#include <vector>
#include <complex>
#include <cmath>
#include <cstdio>
#include <string>

const double PI = 3.14159265358979323846;

std::vector<std::complex<double>> DFT(const std::vector<double>& x) {
    int N = x.size();
    std::vector<std::complex<double>> X(N);
    for (int k = 0; k < N; ++k) {
        std::complex<double> sum(0.0, 0.0);
        for (int n = 0; n < N; ++n) {
            double angle = 2 * PI * k * n / N;
            sum += std::polar(1.0, -angle) * x[n];
        }
        X[k] = sum;
    }
    return X;
}

/*
int nextPowerOfTwo(int n) {
    int power = 1;
    while (power < n) power <<= 1;
    return power;
}

std::vector<std::complex<double>> FFT_recursive(const std::vector<std::complex<double>>& x) {
    int N = x.size();
    if (N == 1) return { x[0] };

    std::vector<std::complex<double>> even(N / 2), odd(N / 2);
    for (int i = 0; i < N / 2; ++i) {
        even[i] = x[2 * i];
        odd[i] = x[2 * i + 1];
    }

    auto Feven = FFT_recursive(even);
    auto Fodd = FFT_recursive(odd);

    std::vector<std::complex<double>> X(N);
    for (int k = 0; k < N / 2; ++k) {
        std::complex<double> twiddle = std::polar(1.0, -2 * PI * k / N) * Fodd[k];
        X[k] = Feven[k] + twiddle;
        X[k + N / 2] = Feven[k] - twiddle;
    }

    return X;
}

std::vector<std::complex<double>> FFT(const std::vector<double>& x) {
    int N = x.size();
    int N_padded = nextPowerOfTwo(N);

    std::vector<std::complex<double>> x_complex(N_padded, std::complex<double>(0.0, 0.0));
    for (int i = 0; i < N; ++i) {
        x_complex[i] = std::complex<double>(x[i], 0.0);
    }

    return FFT_recursive(x_complex);
}
*/

std::vector<double> generateSineWave(double A, double f, double fs, double Tc) {
    int N = static_cast<int>(Tc * fs);
    std::vector<double> x(N);
    for (int n = 0; n < N; ++n) {
        double t = n / fs;
        x[n] = A * sin(2 * PI * f * t); // sin(2 * PI * 1000 * t) + 0.5 * sin(2 * PI * 500 * t); //  
    }
    return x;
}

std::vector<double> calculateAmplitudeSpectrum(const std::vector<std::complex<double>>& X) {
    std::vector<double> M(X.size() / 2);  
    for (int k = 0; k < X.size() / 2; ++k) {
        M[k] = std::abs(X[k]);
    }
    return M;
}

std::vector<double> calculateFrequencyScale(int N, double fs) {
    std::vector<double> f(N / 2);
    for (int k = 0; k < N / 2; ++k) {
        f[k] = k * fs / N;
    }
    return f;
}

void plotSpectrum(const std::string& title, const std::vector<double>& f, const std::vector<double>& M) {
    if (f.size() != M.size()) {
        std::cerr << "Rozmiary wektorów nie są zgodne!" << std::endl;
        return;
    }

    FILE* gp = _popen("gnuplot -persistent", "w");
    if (!gp) {
        std::cerr << "Nie udało się otworzyć Gnuplota!" << std::endl;
        return;
    }

    fprintf(gp, "set terminal pngcairo size 800,600\n");
    fprintf(gp, "set output '%s.png'\n", title.c_str());
    fprintf(gp, "set title 'Widmo amplitudowe sygnału'\n");
    fprintf(gp, "set xlabel 'Częstotliwość (kHz)'\n");
    fprintf(gp, "set ylabel 'Amplituda'\n");

    fprintf(gp, "plot '-' with lines lc rgb 'blue' title 'Widmo'\n");

    for (size_t k = 0; k < f.size(); ++k) {
        fprintf(gp, "%lf %lf\n", f[k]/1000 , M[k] / M.size());
    }
    fprintf(gp, "e\n");

    _pclose(gp);

    std::cout << "Wygenerowano wykres: " << title << std::endl;
}

void plotSignal(const std::string& title, const std::vector<double>& signal, const double fs) {
    FILE* gp = _popen("gnuplot -persistent", "w");

    if (!gp) {
        std::cerr << "Nie udało się otworzyć Gnuplota!" << std::endl;
        return;
    }

    fprintf(gp, "set terminal pngcairo size 800,600\n");
    fprintf(gp, "set output '%s.png'\n", title.c_str());
    fprintf(gp, "set title '%s'\n", title.c_str());
    fprintf(gp, "set xlabel 'Czas [s]'\n");
    fprintf(gp, "set ylabel 'Amplituda'\n");
    fprintf(gp, "plot '-' with lines lc rgb 'blue' title '%s'\n", title.c_str());

    for (size_t i = 0; i < signal.size(); ++i) {
        double t = i / fs;
        fprintf(gp, "%lf %lf\n", t, signal[i]);
    }

    fprintf(gp, "e\n");

    _pclose(gp);

    std::cout << "Wygenerowano wykres: " << title << std::endl;
}

int main() {
    double A = 1.0;         // wysokoszcz wykresu spektrom A/N
    double f = 500.0;       // wartoscz spektrumu
    double fs = 16000.0;    // Częstotliwość próbkowania
    double Tc = 1.0;        // dwugoszcz wykresu sygnalu
            
    std::vector<double> signal = generateSineWave(A, f, fs, Tc);

    std::vector<std::complex<double>> spectrum = DFT(signal);
    //std::vector<std::complex<double>> spectrum_FFT = FFT(signal);

    std::vector<double> M = calculateAmplitudeSpectrum(spectrum);
    //std::vector<double> M_FFT = calculateAmplitudeSpectrum(spectrum_FFT);

    std::vector<double> f_k = calculateFrequencyScale(signal.size(), fs);
    //std::vector<double> f_k_FFT = calculateFrequencyScale(spectrum_FFT.size(), fs);

    plotSignal("sygnal_xt", signal, fs);
    plotSpectrum("spectrum_xt", f_k, M);
    //plotSpectrum("spectrum_xt_FFT", f_k_FFT, M_FFT);

    return 0;
}


