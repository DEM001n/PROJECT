//tabela 1 -> 4
//tabela 2 -> 3

#include <iostream>
#include <fstream>
#include <cmath>

#define PI 3.14159265358979323846



double f = 2.0;
double phi = PI / 2;
double fs = 8000.0;
double Tc = 2.0;
double Ts = 1.0 / fs;
int N = static_cast<int>(Tc * fs);

double x(double t) {
    return sin(2 * PI * f * t - phi + cos(40 * PI * t)) * (pow(1.2, -t / 0.03) + 0.3);
}

double y(double t) {
    return (pow(t, 3) - 1) + cos(4 * t * t * PI) * t;
}

double z(double t) {
    return x(t) / (fabs(y(t) * cos(5 * t) - x(t) * y(t)) + 3);
}

double v(double t) {
    return (x(t) * 662) / (fabs(x(t) - y(t)) + 0.5);
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

int main()
{
    plot("x-signal", x);
    plot("y-signal", y);
    plot("z-signal", z);
    plot("v-signal", v);

    return 0;
}
