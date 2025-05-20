#include <iostream>
#include <vector>
#include <complex>
#include <cmath>
#include <cstdio>
#include <string>
#include <algorithm>

using namespace std;
using Complex = complex<double>;
using CArray = vector<Complex>;
using VectorD = vector<double>;

const double PI = 3.141592653589793238460;

void fft(CArray& a) {
    size_t n = a.size();
    if (n <= 1) return;

    CArray even(n / 2);
    CArray odd(n / 2);
    for (size_t i = 0; i < n / 2; i++) {
        even[i] = a[i * 2];
        odd[i] = a[i * 2 + 1];
    }

    fft(even);
    fft(odd);

    for (size_t i = 0; i < n / 2; i++) {
        Complex t = polar(1.0, -2 * PI * i / n) * odd[i];
        a[i] = even[i] + t;
        a[i + n / 2] = even[i] - t;
    }
}

pair<VectorD, VectorD> oblicz_widmo(const VectorD& sygnal, double fs) {
    size_t N = sygnal.size();
    CArray fft_wartosci(sygnal.begin(), sygnal.end());

    fft(fft_wartosci);

    VectorD czestotliwosci(N / 2);
    VectorD amplituda(N / 2);

    for (size_t i = 0; i < N / 2; i++) {
        czestotliwosci[i] = i * fs / N;
        amplituda[i] = abs(fft_wartosci[i]);
    }

    double max_amp = *max_element(amplituda.begin(), amplituda.end());
    for (auto& a : amplituda) {
        a /= max_amp;
    }

    return { czestotliwosci, amplituda };
}

std::vector<double> generateSineWave(double A, double f, double fs, double Tc) {
    int N = static_cast<int>(Tc * fs);
    std::vector<double> x(N);
    for (int n = 0; n < N; ++n) {
        double t = n / fs;
        x[n] = A * sin(2 * PI * f * t);
    }
    return x;
}

std::vector<double> generateAM(const std::vector<double>& m, double ka, double fn, double fs) {
    int N = m.size();
    std::vector<double> am(N);
    for (int n = 0; n < N; ++n) {
        double t = n / fs;
        am[n] = (ka * m[n] + 1.0) * cos(2 * PI * fn * t);
    }
    return am;
}

std::vector<double> generatePM(const std::vector<double>& m, double kp, double fn, double fs) {
    int N = m.size();
    std::vector<double> pm(N);
    for (int n = 0; n < N; ++n) {
        double t = n / fs;
        pm[n] = cos(2 * PI * fn * t + kp * m[n]);
    }
    return pm;
}

std::vector<double> generateFM(const std::vector<double>& m, double kf, double fm, double fn, double fs) {
    int N = m.size();
    std::vector<double> fm_signal(N);
    for (int n = 0; n < N; ++n) {
        double t = n / fs;
        fm_signal[n] = cos(2 * PI * fn * t + (kf / fm) * m[n]);
    }
    return fm_signal;
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

void plotSignal(const std::string& title, const std::vector<double>& signal1, const std::vector<double>& signal2, const double fs) {
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
    fprintf(gp, "plot '-' with lines lc rgb 'blue' title 'Signal', '-' with lines lc rgb 'red' title '%s'\n", title.c_str());

    for (size_t i = 0; i < signal1.size(); ++i) {
        double t = i / fs;
        fprintf(gp, "%lf %lf\n", t, signal1[i]);
    }
    fprintf(gp, "e\n");

    for (size_t i = 0; i < signal2.size(); ++i) {
        double t = i / fs;
        fprintf(gp, "%lf %lf\n", t, signal2[i]);
    }
    fprintf(gp, "e\n");

    _pclose(gp);

    std::cout << "Wygenerowano wykres: " << title << std::endl;
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

    fprintf(gp, "plot '-' with impulses lc rgb 'blue' title 'Widmo'\n");

    for (size_t k = 0; k < f.size(); ++k) {
        fprintf(gp, "%lf %lf\n", f[k] / 1000, M[k]);
    }
    fprintf(gp, "e\n");

    _pclose(gp);

    std::cout << "Wygenerowano wykres: " << title << std::endl;
}



int main() {
    double fs = 10000;
    double Tc = 2.0;
    double fm = 50;
    double fn = 1000;

    VectorD m = generateSineWave(1.0, fm, fs, Tc);

    VectorD am_mod = generateAM(m, 0.5, fn, fs);
    VectorD pm_mod = generatePM(m, 4 * PI, fn, fs);
    VectorD fm_mod = generateFM(m, 50 * PI, fm, fn, fs);

    pair<VectorD, VectorD> wynik_m = oblicz_widmo(m, fs);
    pair<VectorD, VectorD> wynik_am = oblicz_widmo(am_mod, fs);
    pair<VectorD, VectorD> wynik_pm = oblicz_widmo(pm_mod, fs);
    pair<VectorD, VectorD> wynik_fm = oblicz_widmo(fm_mod, fs);

    plotSpectrum("Widmo Sygnalu", wynik_m.first, wynik_m.second);
    plotSpectrum("Widmo AM", wynik_am.first, wynik_am.second);
    plotSpectrum("Widmo PM", wynik_pm.first, wynik_pm.second);
    plotSpectrum("Widmo FM", wynik_fm.first, wynik_fm.second);

    return 0;
}

