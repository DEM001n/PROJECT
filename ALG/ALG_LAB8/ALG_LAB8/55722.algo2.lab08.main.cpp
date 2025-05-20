//ALGO2 IS1 210B LAB08
//Dmytro Mahaliuk
//md55722@zut.edu.pl

#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <stdexcept>
#include <ctime>

template <typename Func>
double measureExecutionTime(Func func) {
    std::clock_t start = std::clock();
    func();
    std::clock_t end = std::clock();
    double duration = double(static_cast<int64_t>(end) - static_cast<int64_t>(start)) / (CLOCKS_PER_SEC / 1000);
    return duration;
}

// Reprezentacja węzła
struct Node {
    int id;
    double x, y; // Współrzędne węzła
};

// Reprezentacja krawędzi
struct Edge {
    int u, v;    // Węzły połączone krawędzią
    double weight;  // Waga krawędzi

    bool operator<(const Edge& other) const {
        return weight < other.weight;
    }
};

// Union-Find structure z licznikami operacji
class UnionFind {
private:
    std::vector<int> parent;
    std::vector<int> rank;
    bool useRank, usePathCompression;

public:
    int findCount = 0; // Licznik operacji find

    UnionFind(int n, bool useRank = true, bool usePathCompression = true)
        : parent(n), rank(n, 0), useRank(useRank), usePathCompression(usePathCompression) {
        for (int i = 0; i < n; ++i) {
            parent[i] = i;
        }
    }

    int find(int x) {
        ++findCount;
        if (parent[x] != x) {
            if (usePathCompression) {
                parent[x] = find(parent[x]);
            }
            else {
                return find(parent[x]);
            }
        }
        return parent[x];
    }

    void unionSets(int x, int y) {
        int rootX = find(x);
        int rootY = find(y);

        if (rootX != rootY) {
            if (useRank) {
                if (rank[rootX] > rank[rootY]) {
                    parent[rootY] = rootX;
                }
                else if (rank[rootX] < rank[rootY]) {
                    parent[rootX] = rootY;
                }
                else {
                    parent[rootY] = rootX;
                    ++rank[rootX];
                }
            }
            else {
                parent[rootY] = rootX;
            }
        }
    }
};

// Funkcja implementująca algorytm Kruskala
std::vector<Edge> kruskal(int n, const std::vector<Edge>& edges, UnionFind& uf, double& sortTime, double& loopTime) {
    std::vector<Edge> sortedEdges = edges;

    sortTime = measureExecutionTime([&]() {
        // Sortowanie krawędzi z pomiarem czasu
        std::sort(sortedEdges.begin(), sortedEdges.end());
        });

     std::vector<Edge> mst;
       
    loopTime = measureExecutionTime([&]() {    
        // Przetwarzanie krawędzi z pomiarem czasu
        for (const auto& edge : sortedEdges) {
            if (uf.find(edge.u) != uf.find(edge.v)) {
                mst.push_back(edge);
                uf.unionSets(edge.u, edge.v);
                if (mst.size() == n - 1) break;
            }
        }});

    return mst;
}

// Wczytywanie grafu z pliku
int loadGraph(const std::string& filename, std::vector<Node>& nodes, std::vector<Edge>& edges) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file");
    }

    int nodeCount, edgeCount;
    file >> nodeCount;

    nodes.resize(nodeCount);
    for (int i = 0; i < nodeCount; ++i) {
        double x, y;
        file >> x >> y;
        nodes[i] = { i, x, y };
    }

    file >> edgeCount;

    edges.resize(edgeCount);
    for (int i = 0; i < edgeCount; ++i) {
        int u, v;
        double weight;
        file >> u >> v >> weight;
        edges[i] = { u, v, weight };
    }

    return edgeCount;
}
void processGraph(const std::string& filename) {
    std::vector<Node> nodes;
    std::vector<Edge> edges;

    try {
        loadGraph(filename, nodes, edges);

        std::cout << "\nProcessing file: " << filename << "================================\n";

        bool rankOptions[] = { true, false };
        bool pathCompressionOptions[] = { true, false };

        for (bool useRank : rankOptions) {
            for (bool usePathCompression : pathCompressionOptions) {
                UnionFind uf(nodes.size(), useRank, usePathCompression);

                double sortTime = 0, loopTime = 0;
                std::vector<Edge> mst = kruskal(nodes.size(), edges, uf, sortTime, loopTime);

                double mstWeight = 0;
                for (const auto& edge : mst) {
                    mstWeight += edge.weight;
                }

                double findPerNode = (double)uf.findCount / edges.size();

                std::cout << "\nConfiguration: "
                    << "useRank=" << (useRank?"true":"false") << ", usePathCompression=" << (usePathCompression?"true":"false") << "\n";
                std::cout << "Number of edges in MST: " << mst.size() << "\n";
                std::cout << "Total weight of MST: " << mstWeight << "\n";
                std::cout << "Time for sorting edges: " << sortTime << " milliseconds\n";
                std::cout << "Time for main loop: " << loopTime << " milliseconds\n";
                std::cout << "Number of find operations: " << uf.findCount << "\n";
                std::cout << "Number of find operations per edge: " << findPerNode << "\n";
            }
        }
    }
    catch (const std::exception& ex) {
        std::cerr << "Error processing file " << filename << ": " << ex.what() << "\n";
    }
}

int main() {
    std::vector<std::string> filenames = { "g1.txt", "g2.txt", "g3.txt" };

    for (const auto& filename : filenames) {
        processGraph(filename);
    }

    return 0;
}
