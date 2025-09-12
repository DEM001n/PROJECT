#include <iostream>
#include <vector>
#include <complex>
#include <cmath>
#include <cstdio>
#include <string>
#include <fftw3.h >

const double PI = 3.14159265358979323846;

std::vector<double> generateSignalX(double f1, double fs, double Tc) {
    int N = static_cast<int>(Tc * fs);
    std::vector<double> signal(N, 0.0);
    for (int n = 0; n < N; ++n) {
        double t = n / fs;
        signal[n] = 0.5 * std::sin(2 * PI * f1 * t);
    }
    return signal;
}

std::vector<double> generateSignalY(double f1, double f2, double fs, double Tc) {
    int N = static_cast<int>(Tc * fs);
    std::vector<double> signal(N, 0.0);
    for (int n = 0; n < N; ++n) {
        double t = n / fs;
        signal[n] = std::sin(2 * PI * f2 * t) + 0.7 * std::sin(2 * PI * f1 * t);
    }
    return signal;
}

size_t next_power_of_2(size_t n) {
    size_t power = 1;
    while (power < n) {
        power *= 2;
    }
    return power;
}
std::vector<std::complex<double>> fft(const std::vector<std::complex<double>>& input) {
    size_t n = input.size();
    size_t new_size = next_power_of_2(n);
    std::vector<std::complex<double>> padded_input(input);
    padded_input.resize(new_size, std::complex<double>(0.0, 0.0));

    fftw_complex* in = reinterpret_cast<fftw_complex*>(padded_input.data());
    fftw_complex* out = reinterpret_cast<fftw_complex*>(padded_input.data());

    fftw_plan plan = fftw_plan_dft_1d(new_size, in, out, FFTW_FORWARD, FFTW_ESTIMATE);
    fftw_execute(plan);

    std::vector<std::complex<double>> result(new_size);
    for (size_t i = 0; i < new_size; ++i) {
        result[i] = std::complex<double>(out[i][0], out[i][1]);
    }

    fftw_destroy_plan(plan);
    return result;
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
        fprintf(gp, "%lf %lf\n", f[k] / 1000, M[k] / M.size());
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
    double f1 = 100.0;        // Częstotliwość sygnału 1
    double f2 = 150.0;        // Częstotliwość sygnału 2
    double fs = 3000.0;       // Częstotliwość próbkowania
    double Tc = 0.05;          // Czas trwania sygnału

    double a = 1.0;
    double B = 10.0;

    // Generowanie sygnałów
    std::vector<double> signalX = generateSignalX(f1, fs, Tc);
    std::vector<double> signalY = generateSignalY(f1, f2, fs, Tc);

    // Generowanie sygnału z(t) = a*x(t) + B*y(t)
    std::vector<double> signalZ(signalX.size(), 0.0);
    for (size_t i = 0; i < signalX.size(); ++i) {
        signalZ[i] = a * signalX[i] + B * signalY[i];
    }

    // Obliczanie widm dla sygnałów x, y, i z
    std::vector<std::complex<double>> spectrumX = fft({ signalX.begin(), signalX.end() });
    std::vector<std::complex<double>> spectrumY = fft({ signalY.begin(), signalY.end() });
    std::vector<std::complex<double>> spectrumZ = fft({ signalZ.begin(), signalZ.end() });

    std::vector<double> M_x = calculateAmplitudeSpectrum(spectrumX);
    std::vector<double> M_y = calculateAmplitudeSpectrum(spectrumY);
    std::vector<double> M_z = calculateAmplitudeSpectrum(spectrumZ);

    std::vector<double> M_z_prime(M_x.size(), 0.0);
    for (size_t k = 0; k < M_x.size(); ++k) {
        M_z_prime[k] = a * M_x[k] + B * M_y[k];
    }

    std::vector<double> f_x = calculateFrequencyScale(spectrumX.size(), fs);
    std::vector<double> f_y = calculateFrequencyScale(spectrumY.size(), fs);
    std::vector<double> f_z = calculateFrequencyScale(spectrumZ.size(), fs);
    std::vector<double> f_z_prime = calculateFrequencyScale(spectrumX.size(), fs);

    plotSignal("Sygnal X", signalX, fs);
    plotSignal("Sygnal Y", signalY, fs);
    plotSignal("Sygnal Z", signalZ, fs);

    plotSpectrum("Widmo X", f_x, M_x);
    plotSpectrum("Widmo Y", f_y, M_y);
    plotSpectrum("Widmo Z", f_z, M_z);
    plotSpectrum("Widmo Z-prime", f_z_prime, M_z_prime);

    return 0;
}
