#include <iostream>
#include <vector>
#include <complex>
#include <cmath>
#include <cstdio>

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

std::vector<double> generateSineWave(double A, double f, double fs, double Tc) {
    int N = static_cast<int>(Tc * fs);
    std::vector<double> x(N);
    for (int n = 0; n < N; ++n) {
        double t = n / fs;
        x[n] = A * sin(2 * PI * f * t); // sin(2 * PI * 1000 * t) + 0.5 * sin(2 * PI * 500 * t); //  
    }
    return x;
}

std::vector<double> generateSumOfSineWaves(double A, double fs, double Tc, double f1) {
    int N = static_cast<int>(Tc * fs);
    std::vector<double> x(N);
    double f2 = fs / 2 - f1;
    double f3 = f1 / 2;

    for (int n = 0; n < N; ++n) {
        double t = n / fs;
        x[n] = A * sin(2 * PI * f1 * t) + A * sin(2 * PI * f2 * t) + A * sin(2 * PI * f3 * t); // sin(2 * PI * 1000 * t) + 0.5 * sin(2 * PI * 500 * t); //   
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

    for (int k = 0; k < N / 2; ++k)
        f[k] = k * fs / N;
        
    return f;
}

std::vector<double> convertToDecibels(const std::vector<double>& M) {
    std::vector<double> MdB(M.size());
    for (size_t k = 0; k < M.size(); ++k) {
        MdB[k] = 10 * log10( M[k] / M.size() );
    } 

    return MdB;
}

void plotdBSpectrum(const std::string& title, const std::vector<double>& f, const std::vector<double>& M) {
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
    fprintf(gp, "set ylabel 'Decybel'\n");

    fprintf(gp, "plot '-' with lines lc rgb 'blue' title 'Widmo'\n");

    for (size_t k = 0; k < f.size(); ++k) {
        fprintf(gp, "%lf %lf\n", f[k] / 1000, M[k]);
    }
    fprintf(gp, "e\n");

    _pclose(gp);

    std::cout << "Wygenerowano wykres: " << title << std::endl;
}

void plotDoubledBSpectrum(const std::string& title, const std::vector<double>& f, const std::vector<double>& M) {
    if (f.size() != M.size()) {
        std::cerr << "Rozmiary wektorów nie są zgodne!" << std::endl;
        return;
    }

    FILE* gp = _popen("gnuplot -persistent", "w");
    if (!gp) {
        std::cerr << "Nie udało się otworzyć Gnuplota!" << std::endl;
        return;
    }

    fprintf(gp, "set terminal pngcairo size 1200,600\n");
    fprintf(gp, "set output '%s.png'\n", title.c_str());
    fprintf(gp, "set multiplot layout 1,2 title '%s'\n", title.c_str());

    fprintf(gp, "$DANE1 << EOD\n");
    for (size_t k = 0; k < f.size(); ++k) {
        fprintf(gp, "%lf %lf\n", f[k] / 1000, M[k]);
    }
    fprintf(gp, "EOD\n");

    fprintf(gp, "$DANE2 << EOD\n");
    for (size_t k = 0; k < f.size(); ++k) {
        fprintf(gp, "%lf %lf\n", f[k] / 1000, M[k]);
    }
    fprintf(gp, "EOD\n");

    fprintf(gp, "set title 'Widmo dB lin.'\n");
    fprintf(gp, "set xlabel 'Częstotliwość (kHz)'\n");
    fprintf(gp, "set ylabel 'Decybel'\n");
    fprintf(gp, "plot $DANE1 with lines lc rgb 'blue' title 'Widmo dB lin.'\n");

    fprintf(gp, "set title 'Widmo dB log.'\n");
    fprintf(gp, "set xlabel 'Częstotliwość (kHz)'\n");
    fprintf(gp, "set ylabel 'Decybel'\n");
    fprintf(gp, "set logscale x 10\n");
    fprintf(gp, "set xtics (\"0.001\" 0.001, \"0.01\" 0.01, \"0.1\" 0.1, \"1\" 1)\n");
    fprintf(gp, "plot $DANE2 with lines lc rgb 'red' title 'Widmo dB log.'\n");
    fprintf(gp, "unset logscale x\n");
    fprintf(gp, "unset multiplot\n");

    _pclose(gp);

    std::cout << "Wygenerowano podwojny wykres: " << title << std::endl;
}

void plotLinLogSpectrum(const std::string& title, const std::vector<double>& f1, const std::vector<double>& M1, const std::vector<double>& f2, const std::vector<double>& M2) {
    if (f1.size() != M1.size() || f2.size() != M2.size()) {
        std::cerr << "Rozmiary wektorów nie są zgodne!" << std::endl;
        return;
    }

    FILE* gp = _popen("gnuplot -persistent", "w");
    if (!gp) {
        std::cerr << "Nie udało się otworzyć Gnuplota!" << std::endl;
        return;
    }

    fprintf(gp, "set terminal pngcairo size 1200,600\n");
    fprintf(gp, "set output '%s.png'\n", title.c_str());
    fprintf(gp, "set multiplot layout 1,2 title '%s'\n", title.c_str());

    fprintf(gp, "$DANE1 << EOD\n");
    for (size_t k = 0; k < f1.size(); ++k) {
        fprintf(gp, "%lf %lf\n", f1[k] / 1000, M1[k]/M1.size());
    }
    fprintf(gp, "EOD\n");

    fprintf(gp, "$DANE2 << EOD\n");
    for (size_t k = 0; k < f2.size(); ++k) {
        fprintf(gp, "%lf %lf\n", f2[k] / 1000, M2[k]);
    }
    fprintf(gp, "EOD\n");

    fprintf(gp, "set title 'Widmo dB lin.'\n");
    fprintf(gp, "set xlabel 'Częstotliwość (kHz)'\n");
    fprintf(gp, "set ylabel 'Amplituda'\n");
    fprintf(gp, "plot $DANE1 with lines lc rgb 'blue' title 'Widmo lin.'\n");

    fprintf(gp, "set title 'Widmo dB log.'\n");
    fprintf(gp, "set xlabel 'Częstotliwość (kHz)'\n");
    fprintf(gp, "set ylabel 'Decybel'\n");
    fprintf(gp, "plot $DANE2 with lines lc rgb 'red' title 'Widmo log.'\n");
    fprintf(gp, "unset multiplot\n");

    _pclose(gp);

    std::cout << "Wygenerowano podwojny wykres: " << title << std::endl;
}



int main() {
    double A = 1.0;         
    double f = 500.0;       
    double fs = 16000.0;   
    double Tc = 0.1;

    std::vector<double> signal = generateSineWave(A, f, fs, Tc);
    std::vector<std::complex<double>> spectrum = DFT(signal);
    std::vector<double> M = calculateAmplitudeSpectrum(spectrum);
    std::vector<double> f_k = calculateFrequencyScale(signal.size(), fs);
    std::vector<double> MdB = convertToDecibels(M);

    plotdBSpectrum("spectrum-dB", f_k, MdB);

    A = 1.0;
    f = 10.0;
    fs = 2000.0; 
    Tc = 0.3;

    signal = generateSumOfSineWaves(A, fs, Tc, f);
    spectrum = DFT(signal);
    M = calculateAmplitudeSpectrum(spectrum);
    f_k = calculateFrequencyScale(signal.size(), fs);
    MdB = convertToDecibels(M);

    plotDoubledBSpectrum("spectrum-dB-lin-log", f_k, MdB);
    plotLinLogSpectrum("test_spectrum-lin-log",f_k,M ,f_k, MdB);

    return 0;
}
