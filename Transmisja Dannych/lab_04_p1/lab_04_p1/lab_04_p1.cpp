#include <iostream>
#include <vector>
#include <cmath>
#include <fstream>

#define PI 3.141592653589793

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

int main() {
    double fs = 10000;  
    double Tc = 0.1;   
    double fm = 50;     
    double fn = 1000;  

    std::vector<double> m = generateSineWave(1.0, fm, fs, Tc);

    std::vector<double> am_mod_1 = generateAM(m, 0.5, fn, fs);
    std::vector<double> am_mod_2 = generateAM(m, 7, fn, fs);
    std::vector<double> am_mod_3 = generateAM(m, 40, fn, fs);

    std::vector<double> pm_mod_1 = generatePM(m, PI/4, fn, fs);
    std::vector<double> pm_mod_2 = generatePM(m, PI/2, fn, fs);
    std::vector<double> pm_mod_3 = generatePM(m, 4 * PI, fn, fs);

    std::vector<double> fm_mod_1 = generateFM(m, PI/4, fm, fn, fs);
    std::vector<double> fm_mod_2 = generateFM(m, PI/2, fm, fn, fs);
    std::vector<double> fm_mod_3 = generateFM(m, 4 * PI, fm, fn, fs);

    plotSignal("Modulacja_Amplitudy_1", m,am_mod_1, fs);
    plotSignal("Modulacja_Amplitudy_2", m, am_mod_2, fs);
    plotSignal("Modulacja_Amplitudy_3", m, am_mod_3, fs);

    plotSignal("Modulacja_Fazowa_1", m, pm_mod_1, fs);
    plotSignal("Modulacja_Fazowa_2", m, pm_mod_2, fs);
    plotSignal("Modulacja_Fazowa_3", m, pm_mod_3, fs);

    plotSignal("Modulacja_Czestotliwosci_1", m,fm_mod_1, fs);
    plotSignal("Modulacja_Czestotliwosci_2", m, fm_mod_2, fs);
    plotSignal("Modulacja_Czestotliwosci_3", m, fm_mod_3, fs);

    return 0;
}
