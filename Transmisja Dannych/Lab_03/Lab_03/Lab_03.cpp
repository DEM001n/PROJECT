#include <iostream>
#include <vector>
#include <complex>
#include <cmath>
#include <cstdio>
#include <string>
#include <fftw3.h >

const double PI = 3.14159265358979323846;

std::vector<std::complex<double>> dft(const std::vector<double>& x) {
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

    /*
    for (size_t i = 0; i < n; ++i) {
        result[i] /= static_cast<double>(new_size);
    }
    */

    return result;
}

std::vector<double> generateSawtoothWave(double f, double fs, double Tc, int H) {
    int N = static_cast<int>(Tc * fs);
    std::vector<double> signal(N, 0.0);
    for (int n = 0; n < N; ++n) {
        double t = n / fs;
        double sum = 0.0;
        for (int k = 1; k <= H; ++k) {
            sum += (std::pow(-1, k + 1) * (std::sin(2 * PI * k * f * t)) / k);
        }
        signal[n] = 2 / PI * sum;
    }
    return signal;
}

std::vector<double> generateTriangleWave(double f, double fs, double Tc, int H) {
    int N = static_cast<int>(Tc * fs);
    std::vector<double> signal(N, 0.0);
    for (int n = 0; n < N; ++n) {
        double t = n / fs;
        double sum = 0.0;
        for (int k = 1; k <= H; ++k) {
            sum += (std::pow(-1, k - 1) * (std::sin(2 * PI * (2 * k - 1) * f * t)) / std::pow(2 * k - 1, 2));
        }
        signal[n] = 8 / std::pow(PI, 2) * sum;
    }
    return signal;
}

std::vector<double> generateSquareWave(double f, double fs, double Tc, int H) {
    int N = static_cast<int>(Tc * fs);
    std::vector<double> signal(N, 0.0);
    for (int n = 0; n < N; ++n) {
        double t = n / fs;
        double sum = 0.0;
        for (int k = 1; k <= H; ++k) {
            sum += (std::sin(2 * PI * (2 * k - 1) * f * t) / (2 * k - 1));
        }
        signal[n] = 4 / PI * sum;
    }
    return signal;
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
    double f = 200.0;        // Częstotliwość sygnału
    double fs = 3000.0;      // Częstotliwość próbkowania
    double Tc = 0.1;       // Czas trwania sygnału
    //int H = 5;           // Liczba składników w sumach

    std::vector<double> signalSawtooth = generateSawtoothWave(f, fs, Tc, 5);
    std::vector<double> signalTriangle = generateTriangleWave(f, fs, Tc, 3);
    std::vector<double> signalSquare = generateSquareWave(f, fs, Tc, 3);

    /*
   
    std::vector<std::complex<double>> spectrumSawtooth = dft(signalSawtooth);
    std::vector<std::complex<double>> spectrumTriangle = dft(signalTriangle);
    std::vector<std::complex<double>> spectrumSquare = dft(signalSquare);

    */

    std::vector<std::complex<double>> spectrumSawtooth_input(signalSawtooth.begin(), signalSawtooth.end());
    std::vector<std::complex<double>> spectrumTriangle_input(signalTriangle.begin(), signalTriangle.end());
    std::vector<std::complex<double>> spectrumSquare_input(signalSquare.begin(), signalSquare.end());
    std::vector<std::complex<double>>spectrumSawtooth = fft(spectrumSawtooth_input);
    std::vector<std::complex<double>>spectrumTriangle = fft(spectrumTriangle_input);
    std::vector<std::complex<double>>spectrumSquare = fft(spectrumSquare_input);

     

    std::vector<double> M_sawtooth = calculateAmplitudeSpectrum(spectrumSawtooth);
    std::vector<double> M_triangle = calculateAmplitudeSpectrum(spectrumTriangle);
    std::vector<double> M_square = calculateAmplitudeSpectrum(spectrumSquare);

    std::vector<double> f_sawtooth = calculateFrequencyScale(spectrumSawtooth.size(), fs);
    std::vector<double> f_triangle = calculateFrequencyScale(spectrumTriangle.size(), fs);
    std::vector<double> f_square = calculateFrequencyScale(spectrumSquare.size(), fs);

    plotSignal("Sygnal Piloksztaltne", signalSawtooth, fs);
    plotSignal("Sygnal Trojkatne", signalTriangle, fs);
    plotSignal("Sygnal Prostokatne", signalSquare, fs);

    plotSpectrum("Widmo Piloksztaltne", f_sawtooth, M_sawtooth);
    plotSpectrum("Widmo Trojkatne", f_triangle, M_triangle);
    plotSpectrum("Widmo Prostokatne", f_square, M_square);

    return 0;
}
