//Dmytro Mahaliuk 55722

//Wizualizacja wykresów została wykonana przy użyciu kodu GPT

#include <iostream>
#include <vector>
#include <cmath>
#include <complex>
#include <cstdlib>
#include <ctime>
#include <fstream>

#define PI 3.141592653589793

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

std::vector<double> convertToDecibels(const std::vector<double>& M) {
    std::vector<double> MdB(M.size());
    for (size_t k = 0; k < M.size(); ++k) {
        MdB[k] = 10 * log10(M[k]);
        if (MdB[k] < 0)
            MdB[k] = 0;
    }

    return MdB;
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
    fprintf(gp, "set ylabel 'Decybelowe (dB)'\n");

    fprintf(gp, "plot '-' with impulses lc rgb 'blue' title 'Widmo'\n");

    for (size_t k = 0; k < f.size(); ++k) {
        fprintf(gp, "%lf %lf\n", f[k] / 1000, M[k]);
    }
    fprintf(gp, "e\n");

    _pclose(gp);

    std::cout << "Wygenerowano wykres: " << title << std::endl;
}

std::vector<int> textToBits(int B, const std::string& text) {
    std::vector<int> bits;

    for (char c : text) {
        if (c < 32 || c > 127) {
            std::cout << "Znak spoza zakresu ASCII 32–127: '" << c << "' — pomijam.\n";
            continue;
        }

        for (int i = 6; i >= 0; --i) {
            bits.push_back((c >> i) & 1);
            if (bits.size() == static_cast<size_t>(B)) {
                return bits;
            }
        }
    }

    while (bits.size() < static_cast<size_t>(B)) {
        bits.push_back(0);
    }

    return bits;
}

std::vector<double> generateASK(const std::vector<int>& bits, double A1, double A2, double fn, double fs, double Tb) {
    std::vector<double> signal;
    int samplesPerBit = static_cast<int>(Tb * fs);
    for (int b : bits) {
        double A = b == 0 ? A1 : A2;
        for (int n = 0; n < samplesPerBit; ++n) {
            double t = n / fs;
            signal.push_back(A * sin(2 * PI * fn * t));
        }
    }
    return signal;
}

std::vector<double> generateFSK(const std::vector<int>& bits, double fn1, double fn2, double fs, double Tb) {
    std::vector<double> signal;
    int samplesPerBit = static_cast<int>(Tb * fs);
    for (int b : bits) {
        double fn = b == 0 ? fn1 : fn2;
        for (int n = 0; n < samplesPerBit; ++n) {
            double t = n / fs;
            signal.push_back(sin(2 * PI * fn * t));
        }
    }
    return signal;
}

std::vector<double> generatePSK(const std::vector<int>& bits, double fn, double fs, double Tb) {
    std::vector<double> signal;
    int samplesPerBit = static_cast<int>(Tb * fs);
    for (int b : bits) {
        double phase = b == 0 ? 0 : PI;
        for (int n = 0; n < samplesPerBit; ++n) {
            double t = n / fs;
            signal.push_back(sin(2 * PI * fn * t + phase));
        }
    }
    return signal;
}

double estimateBandwidthBPrime(const std::vector<double>& f, const std::vector<double>& M, double fn, double fs, double target_ratio) {

    double total_energy = 0.0;
    for (double val : M) {
        total_energy += val * val;
    }

    double alpha = 0;

    while (alpha <= fs / 2) {
        int fa = fn - alpha;
        int fb = fn + alpha;

        int ia = std::max(0, fa);
        int ib = std::min(static_cast<int>(f.size()) - 1, fb);

        double local_energy = 0.0;
        for (int k = ia; k <= ib; ++k) {
            local_energy += M[k] * M[k];
        }

        double ratio = (local_energy / total_energy) * 100.0;

        if (ratio >= target_ratio) {
            return fb - fa;
        }

        alpha += 1.0;
    }

    return 0.0;
}

void initializeFile(const std::string& filename) {
    std::ofstream outFile(filename, std::ios::out);
    if (!outFile) {
        std::cerr << "Nie można otworzyć pliku: " << filename << std::endl;
        return;
    }

    outFile << "\n";
    outFile.close();
}

void saveBandwidthsToFile(const std::string& label, const std::vector<double>& f, const std::vector<double>& M, const std::string& filename, double fn, double fs, double target_ratio = 80.0) {
    double  B_prime = estimateBandwidthBPrime(f, M, fn, fs, target_ratio);

    std::ofstream outFile(filename, std::ios::app);
    if (!outFile) {
        std::cerr << "Nie można otworzyć pliku: " << filename << std::endl;
        return;
    }

    outFile << label << ": B' = " << B_prime << " Hz\n";

    outFile.close();
}

int main() {

    double Tc = 0.5;
    int B = 10;
    int W = 2;

    std::string input = "Hi!";
    std::vector<int> bits = textToBits(B, input);

    for (int b : bits) {
        std::cout << b << " ";
    }
    std::cout << std::endl;

    double Tb = Tc / B;
    double fn = W / Tb;
    double fn1 = (W + 1) / Tb;
    double fn2 = (W + 2) / Tb;
    double A1 = 1.0, A2 = 2.0;
    double fs = 10 * fn;

    std::vector<double> ask = generateASK(bits, A1, A2, fn, fs, Tb);
    std::vector<double> fsk = generateFSK(bits, fn1, fn2, fs, Tb);
    std::vector<double> psk = generatePSK(bits, fn, fs, Tb);

    auto dftAsk = dft(ask);
    auto dftFsk = dft(fsk);
    auto dftPsk = dft(psk);

    auto ampAsk = convertToDecibels(calculateAmplitudeSpectrum(dftAsk));
    auto ampFsk = convertToDecibels(calculateAmplitudeSpectrum(dftFsk));
    auto ampPsk = convertToDecibels(calculateAmplitudeSpectrum(dftPsk));

    auto freq = calculateFrequencyScale(dftAsk.size(), fs);

    initializeFile("bandwidths.txt");

    saveBandwidthsToFile("ASK", freq, ampAsk, "bandwidths.txt", fn, fs);
    saveBandwidthsToFile("FSK", freq, ampFsk, "bandwidths.txt", fn, fs);
    saveBandwidthsToFile("PSK", freq, ampPsk, "bandwidths.txt", fn, fs);

    return 0;
}
