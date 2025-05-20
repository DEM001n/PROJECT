//Dmytro Mahaliuk 55722

//Wizualizacja wykresów została wykonana przy użyciu kodu GPT

#include <iostream>
#include <vector>
#include <cmath>
#include <complex>
#include <cstdlib>
#include <ctime>

#define PI 3.141592653589793

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
            signal.push_back(-sin(2 * PI * fn * t + phase));
        }
    }
    return signal;
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

//------------------------------------------------------------------------------------------------------------------------

std::vector<double> correlate(const std::vector<double>& signal, double A, double fn, double fs, double phi = 0.0) {
    std::vector<double> x;
    for (size_t n = 0; n < signal.size(); ++n) {
        double t = n / fs;
        double reference = A * sin(2 * PI * fn * t + phi);
        x.push_back(signal[n] * reference);
    }
    return x;
}

std::vector<double> integrate(const std::vector<double>& x, double fs, double Tb) {
    std::vector<double> p;
    int samplesPerBit = static_cast<int>(Tb * fs);
    for (size_t i = 0; i < x.size(); i += samplesPerBit) {
        double sum = 0.0;
        for (size_t j = i; j < i + samplesPerBit && j < x.size(); ++j) {
            sum += x[j];
        }
        p.push_back(sum / fs); // całkowanie = suma * dt (dt = 1/fs)
    }
    return p;
}

std::vector<int> threshold(const std::vector<double>& p, double h) {
    std::vector<int> bits;
    for (double val : p) {
        bits.push_back(val > h ? 1 : 0);
    }
    return bits;
}

std::vector<double> bitsToSignal(const std::vector<int>& bits, double fs, double Tb) {
    std::vector<double> c;
    int samplesPerBit = static_cast<int>(Tb * fs);
    for (int b : bits) {
        for (int i = 0; i < samplesPerBit; ++i) {
            c.push_back(static_cast<double>(b));
        }
    }
    return c;
}

void plotTwoSignals(const std::string& title,
    const std::vector<double>& signal1,
    const std::vector<double>& signal2,
    double fs) {
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
    fprintf(gp, "plot '-' with lines lc rgb 'blue' title 'Sygnał 1', '-' with lines lc rgb 'red' title 'Sygnał 2'\n");

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

    std::cout << "Wygenerowano wykres podwojny: " << title << std::endl;
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
    double fs = 8 * fn;



    std::vector<double> ask = generateASK(bits, A1, A2, fn, fs, Tb);
    std::vector<double> psk = generatePSK(bits, fn, fs, Tb);



    std::vector<double> x_ask = correlate(ask, A2, fn, fs);
    std::vector<double> p_ask = integrate(x_ask, fs, Tb);
    std::vector<int> demodBitsASK = threshold(p_ask, 0.075);
    std::vector<double> c_ask = bitsToSignal(demodBitsASK, fs, Tb);

    plotSignal("ASK", ask, fs);
    plotSignal("ASK_x(t)", x_ask, fs);
    plotSignal("ASK_p(t)", p_ask, 1.0 / Tb); 
    plotSignal("ASK_c(t)", c_ask, fs);



    std::vector<double> x_psk = correlate(psk, 1.0, fn, fs); 
    std::vector<double> p_psk = integrate(x_psk, fs, Tb);
    std::vector<int> demodBitsPSK = threshold(p_psk, 0.0);
    std::vector<double> c_psk = bitsToSignal(demodBitsPSK, fs, Tb);

    plotSignal("PSK", psk, fs);
    plotSignal("PSK_x(t)", x_psk, fs);
    plotSignal("PSK_p(t)", p_psk, 1.0 / Tb);
    plotSignal("PSK_c(t)", c_psk, fs);

    plotTwoSignals("ASK_PSK_c(t)", c_ask, c_psk, fs);

    std::cout << "\nZdemodulowane bity ASK: ";
    for (int b : demodBitsASK) std::cout << b << " ";
    std::cout << std::endl;

    std::cout << "\nZdemodulowane bity PSK: ";
    for (int b : demodBitsPSK) std::cout << b << " ";
    std::cout << std::endl;



    return 0;
}
