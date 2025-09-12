//tabela 3 -> 1
//tabela 4 -> 9

#include <iostream>
#include <fstream>
#include <cmath>

#define PI 3.14159265358979323846



double fs = 8000.0;
double Tc = 1.0;
double Ts = 1.0 / fs;
int N = static_cast<int>(Tc * fs);

double u(double t) {
    if (t >= 0.0 && t < 0.1) {
        return sin(6 * PI * t) * cos(5 * PI * t);
    }
    else if (t >= 0.1 && t < 0.4) {
        return -1.1 * t * cos(41 * PI * t * t);
    }
    else if (t >= 0.4 && t < 0.72) {
        return t * sin(20 * pow(t, 4));
    }
    else if (t >= 0.72 && t < 1.0) {
        return 3.3 * (t - 0.72) * cos(27 * t + 1.3);
    }
    else {
        return 0.0;
    }
}

double b_k(double t, int Hk) {
    double sum = 0.0;
    for (int h = 1; h <= Hk; ++h) {
        sum += pow(-1, h) * (sin(2 * PI * h * t) + cos(6 * PI * h * t));
    }
    return fabs(sum);
}

void plot(const std::string& title, double (*func)(double)) {
    FILE* gp = _popen("gnuplot -persistent", "w");

    if (!gp) {
        std::cerr << "Nie udało się otworzyć Gnuplota!" << std::endl;
        return;
    }

    fprintf(gp, "set terminal pngcairo size 800,600\n");
    fprintf(gp, "set output '%s.png'\n", title.c_str());
    fprintf(gp, "set title '%s(t)'\n", title.c_str());
    fprintf(gp, "set xlabel 't'\n");
    fprintf(gp, "set ylabel 'value'\n");
    fprintf(gp, "plot '-' with lines lc rgb 'blue' title '%s(t)'\n", title.c_str());

    for (int n = 0; n < N; ++n) {
        double t = n * Ts;
        double val = func(t);
        fprintf(gp, "%lf %lf\n", t, val);
    }
    fprintf(gp, "e\n");

    _pclose(gp);

    std::cout << "wygenerowany " << title << std::endl;
}

void plotB(const std::string& title, int Hk) {
    FILE* gp = _popen("gnuplot -persistent", "w");

    if (!gp) {
        std::cerr << "Nie udało się otworzyć Gnuplota!" << std::endl;
        return;
    }

    fprintf(gp, "set terminal pngcairo size 1000,700\n");
    fprintf(gp, "set output '%s.png'\n", title.c_str());
    fprintf(gp, "set title '%s'\n", title.c_str());
    fprintf(gp, "set xlabel 't [s]'\n");
    fprintf(gp, "set ylabel 'b_k(t)'\n");
    fprintf(gp, "plot '-' with lines lc rgb 'dark-green' title '%s'\n", title.c_str());

    for (int n = 0; n < N; ++n) {
        double t = n * Ts;
        fprintf(gp, "%lf %lf\n", t, b_k(t, Hk));
    }
    fprintf(gp, "e\n");

    _pclose(gp);

    std::cout << "wygenerowany " << title << std::endl;
}

int main()
{
    plot("u-signal", u);

    Ts = 1.0 / 22050.0;
    N = 22050;

    plotB("b1-signal-H2", 2);
    plotB("b2-signal-H20", 20);
    plotB("b3-signal-H60", 60);

    return 0;
}