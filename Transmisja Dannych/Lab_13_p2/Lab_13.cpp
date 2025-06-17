//Dmytro Mahaliuk 55722

//Wizualizacja wykresów została wykonana przy użyciu kodu GPT

//Kod Hamminga 1511 został zaimplementowany na podstawie kodu źródłowego, pochodzącego z https://www.geeksforgeeks.org/hamming-code-implementation-in-python/


#include <iostream>
#include <vector>
#include <cmath>
#include <complex>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <random>

#define PI 3.141592653589793

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

std::vector<int> signalToBits(const std::vector<double>& c, double fs, double Tb) {
    std::vector<int> bits;
    int samplesPerBit = static_cast<int>(Tb * fs);
    for (size_t i = 0; i + samplesPerBit <= c.size(); i += samplesPerBit) {
        double sum = 0.0;
        for (int j = 0; j < samplesPerBit; ++j) {
            sum += c[i + j];
        }
        double avg = sum / samplesPerBit;
        bits.push_back(avg > 0.5 ? 1 : 0);
    }
    return bits;
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
    fprintf(gp, "set yrange [-0.5:1.5]\n");
    fprintf(gp, "plot '-' with lines dashtype 2 lc rgb 'blue' title 'Sygnał 1', '-' with lines lc rgb 'red' title 'Sygnał 2'\n");

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

//--------------------------------------------------------------------------------------------------------------------------------

double calculateBER(const std::vector<int>& original, const std::vector<int>& received) {

    int errors = 0;
    for (size_t i = 0; i < original.size(); ++i) {
        if (original[i] != received[i]) {
            ++errors;
        }
    }

    double ber = (static_cast<double>(errors) / original.size()) * 100.0;
    return ber;
}

void plotSignalBER(const std::string& title, const std::vector<double>& signal, const std::vector<double>& fs) {
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
        fprintf(gp, "%lf %lf\n", fs[i], signal[i]);
    }
    fprintf(gp, "e\n");

    _pclose(gp);

    std::cout << "Wygenerowano wykres: " << title << std::endl;
}

//------------------------------------------------------------------------------------------------------------------------------------

int xor3(int a, int b, int c) {
    return a ^ b ^ c;
}

std::vector<int> hamming74_encode(const std::vector<int>& b_in) {
    std::vector<int> b_res;
    size_t n = b_in.size();

    for (size_t i = 0; i < n; i += 4) {
        std::vector<int> b_buf(7);

        b_buf[2] = b_in[i];
        b_buf[4] = b_in[i + 1];
        b_buf[5] = b_in[i + 2];
        b_buf[6] = b_in[i + 3];

        b_buf[0] = xor3(b_buf[2], b_buf[4], b_buf[6]);
        b_buf[1] = xor3(b_buf[2], b_buf[5], b_buf[6]);
        b_buf[3] = xor3(b_buf[4], b_buf[5], b_buf[6]);

        b_res.insert(b_res.end(), b_buf.begin(), b_buf.end());
    }

    return b_res;
}

std::vector<int> hamming74_decode(const std::vector<int>& b_in) {
    std::vector<int> b_res;
    size_t n = b_in.size();

    for (size_t i = 0; i < n; i += 7) {
        std::vector<int> b(b_in.begin() + i, b_in.begin() + i + 7);

        int x1 = b[0] ^ xor3(b[2], b[4], b[6]);
        int x2 = b[1] ^ xor3(b[2], b[5], b[6]);
        int x3 = b[3] ^ xor3(b[4], b[5], b[6]);

        int S = (x1 + (x2 << 1) + (x3 << 2)) - 1;

        if (S >= 0 && S < 7) {
            b[S] ^= 1;
        }

        b_res.push_back(b[2]);
        b_res.push_back(b[4]);
        b_res.push_back(b[5]);
        b_res.push_back(b[6]);
    }

    return b_res;
}

//-------------------------------------------------------------------------------------------------------

int calcRedundantBits(int m) {
    for (int i = 0; i < m; ++i) {
        if (pow(2, i) >= m + i + 1)
            return i;
    }
    return 0;
}

std::vector<int> posRedundantBits(const std::vector<int>& data, int r) {
    int j = 0, k = 1;
    int m = data.size();
    std::vector<int> res;

    for (int i = 1; i <= m + r; ++i) {
        if (i == (1 << j)) {
            res.push_back(0);
            j++;
        }
        else {
            res.push_back(data[m - k]);
            k++;
        }
    }

    std::reverse(res.begin(), res.end());
    return res;
}

std::vector<int> calcParityBits(std::vector<int> arr, int r) {
    int n = arr.size();

    for (int i = 0; i < r; ++i) {
        int val = 0;
        for (int j = 1; j <= n; ++j) {
            if ((j & (1 << i)) == (1 << i)) {
                val ^= arr[n - j];
            }
        }
        arr[n - (1 << i)] = val;
    }

    return arr;
}

int detectError(const std::vector<int>& arr, int r) {
    int n = arr.size();
    int res = 0;

    for (int i = 0; i < r; ++i) {
        int val = 0;
        for (int j = 1; j <= n; ++j) {
            if ((j & (1 << i)) == (1 << i)) {
                val ^= arr[n - j];
            }
        }
        res += val * pow(10, i);
    }

    int decimal = 0, base = 1, temp = res;
    while (temp > 0) {
        int last_digit = temp % 10;
        temp /= 10;
        decimal += last_digit * base;
        base *= 2;
    }

    return decimal;
}

std::vector<int> hamming1511_encode(const std::vector<int>& data) {
    int m = data.size();
    int r = calcRedundantBits(m);

    std::vector<int> arranged = posRedundantBits(data, r);
    std::vector<int> encoded = calcParityBits(arranged, r);

    return encoded;
}

std::vector<int> hamming1511_decode(const std::vector<int>& encoded) {
    int r = calcRedundantBits(encoded.size() - calcRedundantBits(encoded.size()));

    int error_pos = detectError(encoded, r);
    std::vector<int> corrected = encoded;

    if (error_pos > 0 && error_pos <= encoded.size()) {
        corrected[corrected.size() - error_pos] ^= 1;
    }

    std::vector<int> decoded;
    for (int i = 1; i <= corrected.size(); ++i) {
        if ((i & (i - 1)) != 0) {
            decoded.push_back(corrected[corrected.size() - i]);
        }
    }

    std::reverse(decoded.begin(), decoded.end());
    return decoded;
}

//-------------------------------------------------------------------------------------------------------------------------------------

std::vector<double> addImpulseToSignal(std::vector<double>& x, int K) {

    std::mt19937 rng(static_cast<unsigned>(std::time(nullptr)));
    std::uniform_int_distribution<size_t> dist_pos(0, x.size() - 1);
    std::uniform_real_distribution<double> dist_amp(-1.0, 1.0);

    for (int i = 0; i < K; ++i) {
        size_t pos = dist_pos(rng);   
        double A = dist_amp(rng);     
        x[pos] += A;                
    }

    return x;
}

int main() {
    double Tc = 0.5;
    int W = 2;
    int B = 10;

    std::vector<int> bits = { 1,0,1,1,0,1,0,1,1,1,0,0,1,0,1,0,0,1,1,0,0,1,1,0,1,1,0,0,1,0,1,1,1,0,0,1,1,0,0,1,0,1,0,0,1,1,1,0,1,0,0,1,1,0,0,1,0,1,1,0,0,1,1,0,1,0,1,1,0,1,0,1,0,0,1,1,1,0,1,0,0,1,0,1,0,1,0,1 };

    std::vector<int> bits74 = hamming74_encode(bits);

    double Tb = Tc / B;
    double fn = W / Tb;
    double fn1 = (W + 1) / Tb;
    double fn2 = (W + 2) / Tb;
    double A1 = 1.0, A2 = 2.0;
    double fs = 8 * fn;

    std::vector<double> A_values;
    std::vector<double> ask_ber_results_74;

    for (double a = 0.0; a <= 100; a += 10) {
        A_values.push_back(a);

        std::vector<double> ask_clean = generateASK(bits74, A1, A2, fn, fs, Tb);
        std::vector<double> ask_noisy = addImpulseToSignal(ask_clean,a);

        std::vector<double> x_ask = correlate(ask_noisy, A2, fn, fs);
        std::vector<double> p_ask = integrate(x_ask, fs, Tb);
        std::vector<int> demodBitsASK = threshold(p_ask, 0.075);
        std::vector<int> decodeDemodBitsASK = hamming74_decode(demodBitsASK);

        double ber = calculateBER(bits, decodeDemodBitsASK);
        ask_ber_results_74.push_back(ber);
    }

    plotSignalBER("ASK_hamm74", ask_ber_results_74, A_values);

    std::vector<double> psk_ber_results_74;

    for (double a = 0.0; a <= 100; a += 10) {
        std::vector<double> psk_clean = generatePSK(bits74, fn, fs, Tb);
        std::vector<double> psk_noisy = addImpulseToSignal(psk_clean, a);

        std::vector<double> x_psk = correlate(psk_noisy, 1.0, fn, fs);
        std::vector<double> p_psk = integrate(x_psk, fs, Tb);
        std::vector<int> demodBitsPSK = threshold(p_psk, 0.0);
        std::vector<int> decodeDemodBitsPSK = hamming74_decode(demodBitsPSK);

        double ber = calculateBER(bits, decodeDemodBitsPSK);
        psk_ber_results_74.push_back(ber);
    }

    plotSignalBER("PSK_hamm74", psk_ber_results_74, A_values);

    std::vector<double> fsk_ber_results_74;

    for (double a = 0.0; a <= 100; a += 10) {
        std::vector<double> fsk_clean = generateFSK(bits74, fn1, fn2, fs, Tb);
        std::vector<double> fsk_noisy = addImpulseToSignal(fsk_clean,a);


        std::vector<double> x1_fsk = correlate(fsk_noisy, A2, fn1, fs);
        std::vector<double> x2_fsk = correlate(fsk_noisy, A2, fn2, fs);
        std::vector<double> p1_fsk = integrate(x1_fsk, fs, Tb);
        std::vector<double> p2_fsk = integrate(x2_fsk, fs, Tb);

        std::vector<double> p_fsk;
        for (size_t i = 0; i < p1_fsk.size(); ++i) {
            p_fsk.push_back(p2_fsk[i] - p1_fsk[i]);
        }

        std::vector<int> demodBitsFSK = threshold(p_fsk, 0);
        std::vector<int> decodeDemodBitsFSK = hamming74_decode(demodBitsFSK);

        double ber = calculateBER(bits, decodeDemodBitsFSK);
        fsk_ber_results_74.push_back(ber);
    }

    plotSignalBER("FSK_hamm74", fsk_ber_results_74, A_values);

    //------------------------------------------------------------------------------------------------------

    std::vector<int> bits1511 = hamming1511_encode(bits);

    std::vector<double> ask_ber_results_1511;

    for (double a = 0.0; a <= 100; a += 10) {
        std::vector<double> ask_clean = generateASK(bits1511, A1, A2, fn, fs, Tb);
        std::vector<double> ask_noisy = addImpulseToSignal(ask_clean,a);

        std::vector<double> x_ask = correlate(ask_noisy, A2, fn, fs);
        std::vector<double> p_ask = integrate(x_ask, fs, Tb);
        std::vector<int> demodBitsASK = threshold(p_ask, 0.075);
        std::vector<int> decodeDemodBitsASK = hamming1511_decode(demodBitsASK);

        double ber = calculateBER(bits, decodeDemodBitsASK);
        ask_ber_results_1511.push_back(ber);
    }

    plotSignalBER("ASK_hamm1511", ask_ber_results_1511, A_values);

    std::vector<double> psk_ber_results_1511;

    for (double a = 0.0; a <= 100; a += 10) {
        std::vector<double> psk_clean = generatePSK(bits1511, fn, fs, Tb);
        std::vector<double> psk_noisy = addImpulseToSignal(psk_clean,a);

        std::vector<double> x_psk = correlate(psk_noisy, 1.0, fn, fs);
        std::vector<double> p_psk = integrate(x_psk, fs, Tb);
        std::vector<int> demodBitsPSK = threshold(p_psk, 0.0);
        std::vector<int> decodeDemodBitsPSK = hamming1511_decode(demodBitsPSK);

        double ber = calculateBER(bits, decodeDemodBitsPSK);
        psk_ber_results_1511.push_back(ber);
    }

    plotSignalBER("PSK_hamm1511", psk_ber_results_1511, A_values);

    std::vector<double> fsk_ber_results_1511;

    for (double a = 0.0; a <= 100; a += 10) {
        std::vector<double> fsk_clean = generateFSK(bits1511, fn1, fn2, fs, Tb);
        std::vector<double> fsk_noisy = addImpulseToSignal(fsk_clean, a);


        std::vector<double> x1_fsk = correlate(fsk_noisy, A2, fn1, fs);
        std::vector<double> x2_fsk = correlate(fsk_noisy, A2, fn2, fs);
        std::vector<double> p1_fsk = integrate(x1_fsk, fs, Tb);
        std::vector<double> p2_fsk = integrate(x2_fsk, fs, Tb);

        std::vector<double> p_fsk;
        for (size_t i = 0; i < p1_fsk.size(); ++i) {
            p_fsk.push_back(p2_fsk[i] - p1_fsk[i]);
        }

        std::vector<int> demodBitsFSK = threshold(p_fsk, 0);
        std::vector<int> decodeDemodBitsFSK = hamming1511_decode(demodBitsFSK);

        /*
        std::vector<double> c_ask = bitsToSignal(decodeDemodBitsFSK, fs, Tb);

        plotSignal("test b_noise", fsk_noisy, fs);

        plotSignal("test b_x1", x1_fsk, fs);
        plotSignal("test b_x2", x2_fsk, fs);

        plotSignal("test b_p", p_fsk, 1.0 / Tb);

        plotSignal("test b_c", c_ask, fs);

        std::vector<double> c = bitsToSignal(bits, fs, Tb);
        plotTwoSignals("FSK_BITS_c(t)", c_ask, c, fs);
        */

        double ber = calculateBER(bits, decodeDemodBitsFSK);
        fsk_ber_results_1511.push_back(ber);
    }

    plotSignalBER("FSK_hamm1511", fsk_ber_results_1511, A_values);

    return 0;
}


