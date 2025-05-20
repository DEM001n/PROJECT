#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>
#include <ctime>
#include <cmath>


template <typename Func>
double measureExecutionTime(Func func)
{
    std::clock_t start = std::clock();
    func();
    std::clock_t end = std::clock();
    double duration = double(static_cast<int64_t>(end) - static_cast<int64_t>(start)) / (CLOCKS_PER_SEC / 1000);
    return duration;
};

struct Point {
    int index;
    double x, y;

    Point(double x = 0, double y = 0, int index = -1) : x(x), y(y), index(index) {}
};

double distanceSquared(const Point& p) {
    return sqrt(pow(p.x, 2) + pow(p.y, 2));
}

double crossProduct(const Point& p1, const Point& p2) {
    return p1.x * p2.y - p1.y * p2.x;
}

bool compare(const Point& p1, const Point& p2) {
    double cp = crossProduct(p1, p2);
    if (cp == 0) {
        return distanceSquared(p1) < distanceSquared(p2);
    }
    return cp > 0;
}

std::vector<Point> grahamScan(std::vector<Point>& points) {

    auto it = std::min_element(points.begin(), points.end(), [](const Point& a, const Point& b) {
        return a.y < b.y || (a.y == b.y && a.x < b.x);
        });
    Point start = *it;

    for (auto& p : points) {
        p.x -= start.x;
        p.y -= start.y;
    }

    double sorttime = measureExecutionTime([&]() {std::sort(points.begin(), points.end(), compare); });
    //for (auto& p : points) std::cout << p.index << " ";

    printf("\n");
    
    for (auto& p : points) {
        p.x += start.x;
        p.y += start.y;
    }

    double looptime = measureExecutionTime([&]() {
        for (size_t i = 2; i < points.size(); ++i) {
            Point secondLast = points[i - 2];
            Point last = points[i-1];
            Point current = points[i];

            Point vector1(last.x - secondLast.x, last.y - secondLast.y);
            Point vector2(current.x - last.x, current.y - last.y);

            double cp = crossProduct(vector1, vector2);

            if (cp <= 0) {
                points.erase(points.begin() + (i - 1));
                i-=2;
            } 
        }
        });

    std::cout << "Czas sortowania: " << sorttime << " milliseconds\n";
    std::cout << "Czas glownej petli: " << looptime << " milliseconds\n";

    return points;
}

std::vector<Point> readPointsFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Nie udalo sie otworzyc pliku: " + filename);
    }

    int n;
    file >> n;
    std::vector<Point> points;
    for (int i = 0; i < n; ++i) {
        double x, y;
        file >> x >> y;
        points.emplace_back(x, y, i);
    }
    return points;
}

int main() {
    try {
        std::string filenames[] = { "points1.txt", "points2.txt", "points3.txt", "points4.txt", "points5.txt" };

        for (const auto& filename : filenames) {
            std::cout << "\nPrzetwarzanie pliku: " << filename << "\n";
            auto points = readPointsFromFile(filename);

            std::vector<Point> hull;
            double realtime = measureExecutionTime([&]() {hull = grahamScan(points); });

            std::cout << "Liczba punktow w otoczce wypuklej: " << hull.size() << "\n";
            std::cout << "Indeksy punktow: ";
            for (auto p : hull) {
                std::cout << p.index << " ";
            }
            std::cout << "\nLaczny czas wykonania: " << realtime << " milliseconds\n";
        }

    }
    catch (const std::exception& e) {
        std::cerr << "Blad: " << e.what() << "\n";
    }

    return 0;
}