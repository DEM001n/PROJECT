//Dmytro Mahaliuk 55722

#include <iostream>
#include <vector>
#include <complex>
#include <cmath>
#include <cstdio>
#include <string>
#include <fstream>
//#include <fftw3.h >



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

/*

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

    // for (size_t i = 0; i < n; ++i) result[i] /= static_cast<double>(new_size);

    return result;
}

*/

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

std::vector<double> convertToDecibels(const std::vector<double>& M) {
    std::vector<double> MdB(M.size());
    for (size_t k = 0; k < M.size(); ++k) {
        MdB[k] = 10 * log10(M[k]);
        if (MdB[k] < 0)
            MdB[k] = 0;
    }

    return MdB;
}

double calculateBandwidth(const std::vector<double>& f, const std::vector<double>& M_dB, double dB_down) {
    double max_val = *std::max_element(M_dB.begin(), M_dB.end());
    double threshold = max_val - dB_down;

    double fmin = 0, fmax = 0;
    bool found = false;

    for (size_t i = 0; i < M_dB.size(); ++i) {
        if (M_dB[i] >= threshold) {
            if (!found) {
                fmin = f[i];
                found = true;
            }
            fmax = f[i];
        }
    }

    if (!found || fmin == fmax)
        return 0.0;

    return fmax - fmin;
}

void initializeFile(const std::string& filename) {
    std::ofstream outFile(filename, std::ios::out);
    if (!outFile) {
        std::cerr << "Nie można otworzyć pliku: " << filename << std::endl;
        return;
    }

    outFile << "wnioski:\n";
    outFile << "Dla modulacji AM metoda ta okazuje sie malo efektywna, poniewaz w wiekszosci przypadkow nawet poziom -10 dB nie jest wystarczajacy, aby osiegnac komponentow sygnału po modulacji. Wynika to z bardzo silnej skladowej nosnej znajdujacej sie w centrum widma.\n";
    outFile << "Przypadki, w ktorych udaje się poprawnie okreslic szerokosc pasma, pojawiaja sie tylko wtedy, gdy energia sygnału znajduje sie powyżej centralnej skladowej — jednak sa to sytuacje nietypowe i odpowiadaja slabo zamodulowanym sygnalom.\n";

    outFile << "Dla modulacji PM można zauwazyc, ze metoda ta dziala znacznie lepiej — wyznaczone szerokosci pasma sa spójne i znajduja sie w logicznych granicach, szczegolnie w porownaniu do wynikow dla AM.\n";

    outFile << "Dla modulacji FM widac bardzo słabe wyniki. Skuteczność tej metody w przypadku FM silnie zalezy od początkowych parametrow sygnalu. Przy odpowiednio niskiej czestotliwosci modulujacej lub bardzo duzym wspolczynniku modulacji, wyniki moga przypominac te z PM.\n";
    outFile << "Jednak w sytuacjach, gdy dane poczatkowe sa nieoptymalne (jak w naszym przypadku), centralny pik nosnej jest tak silny, że wartosci sygnalu po modulacji sa zbyt slabe, by osiagnac nawet poziom - 10 dB, przez co nie sa uwzgledniane w analizie.\n";

    outFile << "\n";
    outFile.close();
}

void saveBandwidthsToFile(const std::string& label, const std::vector<double>& f, const std::vector<double>& M_dB, const std::string& filename) {
    double B3dB = calculateBandwidth(f, M_dB, 3.0);
    double B6dB = calculateBandwidth(f, M_dB, 6.0);
    double B10dB = calculateBandwidth(f, M_dB, 10.0);

    std::ofstream outFile(filename, std::ios::app);
    if (!outFile) {
        std::cerr << "Nie można otworzyć pliku: " << filename << std::endl;
        return;
    }

    outFile << label << ":\n";
    outFile << "  B3dB  = " << B3dB << " Hz\n";
    outFile << "  B6dB  = " << B6dB << " Hz\n";
    outFile << "  B10dB = " << B10dB << " Hz\n\n";

    outFile.close();
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
    fprintf(gp, "set ylabel 'Decebel (dB)'\n");
    fprintf(gp, "set xtics (\"0.9\" 0.9, \"0.95\" 0.95, \"1\" 1.0, \"1.05\" 1.05, \"1.1\" 1.1)\n");

    fprintf(gp, "plot '-' with impulses lw 2 lc rgb 'blue' title 'Widmo'\n");

    for (size_t k = 0; k < f.size(); ++k) {
        fprintf(gp, "%lf %lf\n", f[k] / 1000, M[k]);
    }
    fprintf(gp, "e\n");

    _pclose(gp);

    std::cout << "Wygenerowano wykres: " << title << std::endl;
}




int main() {
    double fs = 10000;
    double Tc = 0.5;
    double fm = 50;
    double fn = 1000;



    std::vector<double> m = generateSineWave(1.0, fm, fs, Tc);

    std::vector<double> am_mod_1 = generateAM(m, 0.5, fn, fs);
    std::vector<double> am_mod_2 = generateAM(m, 7, fn, fs);
    std::vector<double> am_mod_3 = generateAM(m, 40, fn, fs);

    std::vector<double> pm_mod_1 = generatePM(m, PI / 4, fn, fs);
    std::vector<double> pm_mod_2 = generatePM(m, PI / 2, fn, fs);
    std::vector<double> pm_mod_3 = generatePM(m, 4 * PI, fn, fs);

    std::vector<double> fm_mod_1 = generateFM(m, PI / 4, fm, fn, fs);
    std::vector<double> fm_mod_2 = generateFM(m, PI / 2, fm, fn, fs);
    std::vector<double> fm_mod_3 = generateFM(m, 4 * PI, fm, fn, fs);



    std::vector<std::complex<double>> spectrum_am_1 = dft(am_mod_1);
    std::vector<std::complex<double>> spectrum_am_2 = dft(am_mod_2);
    std::vector<std::complex<double>> spectrum_am_3 = dft(am_mod_3);

    std::vector<std::complex<double>> spectrum_pm_1 = dft(pm_mod_1);
    std::vector<std::complex<double>> spectrum_pm_2 = dft(pm_mod_2);
    std::vector<std::complex<double>> spectrum_pm_3 = dft(pm_mod_3);

    std::vector<std::complex<double>> spectrum_fm_1 = dft(fm_mod_1);
    std::vector<std::complex<double>> spectrum_fm_2 = dft(fm_mod_2);
    std::vector<std::complex<double>> spectrum_fm_3 = dft(fm_mod_3);


    /*
    std::vector<std::complex<double>> am_imput_1(am_mod_1.begin(), am_mod_1.end());
    std::vector<std::complex<double>> am_imput_2(am_mod_2.begin(), am_mod_2.end());
    std::vector<std::complex<double>> am_imput_3(am_mod_3.begin(), am_mod_3.end());

    std::vector<std::complex<double>> pm_imput_1(pm_mod_1.begin(), pm_mod_1.end());
    std::vector<std::complex<double>> pm_imput_2(pm_mod_2.begin(), pm_mod_2.end());
    std::vector<std::complex<double>> pm_imput_3(pm_mod_3.begin(), pm_mod_3.end());

    std::vector<std::complex<double>> fm_imput_1(fm_mod_1.begin(), fm_mod_1.end());
    std::vector<std::complex<double>> fm_imput_2(fm_mod_2.begin(), fm_mod_2.end());
    std::vector<std::complex<double>> fm_imput_3(fm_mod_3.begin(), fm_mod_3.end());

    std::vector<std::complex<double>>spectrum_am_1 = fft(am_imput_1);
    std::vector<std::complex<double>>spectrum_am_2 = fft(am_imput_2);
    std::vector<std::complex<double>>spectrum_am_3 = fft(am_imput_3);

    std::vector<std::complex<double>>spectrum_pm_1 = fft(pm_imput_1);
    std::vector<std::complex<double>>spectrum_pm_2 = fft(pm_imput_2);
    std::vector<std::complex<double>>spectrum_pm_3 = fft(pm_imput_3);

    std::vector<std::complex<double>>spectrum_fm_1 = fft(fm_imput_1);
    std::vector<std::complex<double>>spectrum_fm_2 = fft(fm_imput_2);
    std::vector<std::complex<double>>spectrum_fm_3 = fft(fm_imput_3);
    */


    std::vector<double> M_am_1_A = calculateAmplitudeSpectrum(spectrum_am_1);
    std::vector<double> M_am_2_A = calculateAmplitudeSpectrum(spectrum_am_2);
    std::vector<double> M_am_3_A = calculateAmplitudeSpectrum(spectrum_am_3);

    std::vector<double> M_pm_1_A = calculateAmplitudeSpectrum(spectrum_pm_1);
    std::vector<double> M_pm_2_A = calculateAmplitudeSpectrum(spectrum_pm_2);
    std::vector<double> M_pm_3_A = calculateAmplitudeSpectrum(spectrum_pm_3);

    std::vector<double> M_fm_1_A = calculateAmplitudeSpectrum(spectrum_fm_1);
    std::vector<double> M_fm_2_A = calculateAmplitudeSpectrum(spectrum_fm_2);
    std::vector<double> M_fm_3_A = calculateAmplitudeSpectrum(spectrum_fm_3);



    std::vector<double> M_am_1 = convertToDecibels(M_am_1_A);
    std::vector<double> M_am_2 = convertToDecibels(M_am_2_A);
    std::vector<double> M_am_3 = convertToDecibels(M_am_3_A);

    std::vector<double> M_pm_1 = convertToDecibels(M_pm_1_A);
    std::vector<double> M_pm_2 = convertToDecibels(M_pm_2_A);
    std::vector<double> M_pm_3 = convertToDecibels(M_pm_3_A);

    std::vector<double> M_fm_1 = convertToDecibels(M_fm_1_A);
    std::vector<double> M_fm_2 = convertToDecibels(M_fm_2_A);
    std::vector<double> M_fm_3 = convertToDecibels(M_fm_3_A);



    std::vector<double> f_am_1 = calculateFrequencyScale(spectrum_am_1.size(), fs);
    std::vector<double> f_am_2 = calculateFrequencyScale(spectrum_am_2.size(), fs);
    std::vector<double> f_am_3 = calculateFrequencyScale(spectrum_am_3.size(), fs);

    std::vector<double> f_pm_1 = calculateFrequencyScale(spectrum_pm_1.size(), fs);
    std::vector<double> f_pm_2 = calculateFrequencyScale(spectrum_pm_2.size(), fs);
    std::vector<double> f_pm_3 = calculateFrequencyScale(spectrum_pm_3.size(), fs);

    std::vector<double> f_fm_1 = calculateFrequencyScale(spectrum_fm_1.size(), fs);
    std::vector<double> f_fm_2 = calculateFrequencyScale(spectrum_fm_2.size(), fs);
    std::vector<double> f_fm_3 = calculateFrequencyScale(spectrum_fm_3.size(), fs);



    plotSpectrum("Widmo AM_1", f_am_1, M_am_1);
    plotSpectrum("Widmo AM_2", f_am_2, M_am_2);
    plotSpectrum("Widmo AM_3", f_am_3, M_am_3);

    plotSpectrum("Widmo PM_1", f_pm_1, M_pm_1);
    plotSpectrum("Widmo PM_2", f_pm_2, M_pm_2);
    plotSpectrum("Widmo PM_3", f_pm_3, M_pm_3);

    plotSpectrum("Widmo FM_1", f_fm_1, M_fm_1);
    plotSpectrum("Widmo FM_2", f_fm_2, M_fm_2);
    plotSpectrum("Widmo FM_3", f_fm_3, M_fm_3);



    initializeFile("bandwidths.txt");

    saveBandwidthsToFile("AM ka=0.5", f_am_1, M_am_1, "bandwidths.txt");
    saveBandwidthsToFile("AM ka=7", f_am_2, M_am_2, "bandwidths.txt");
    saveBandwidthsToFile("AM ka=40", f_am_3, M_am_3, "bandwidths.txt");

    saveBandwidthsToFile("PM kp=PI/4", f_pm_1, M_pm_1, "bandwidths.txt");
    saveBandwidthsToFile("PM kp=PI/2", f_pm_2, M_pm_2, "bandwidths.txt");
    saveBandwidthsToFile("PM kp=4PI", f_pm_3, M_pm_3, "bandwidths.txt");

    saveBandwidthsToFile("FM kf=PI/4", f_fm_1, M_fm_1, "bandwidths.txt");
    saveBandwidthsToFile("FM kf=PI/2", f_fm_2, M_fm_2, "bandwidths.txt");
    saveBandwidthsToFile("FM kf=4PI", f_fm_3, M_fm_3, "bandwidths.txt");



    return 0;
}

