//Dmytro Mahaliuk 55722

#include <iostream>
#include <vector>

const int P[11][4] = {
    {1, 1, 0, 0},
    {1, 0, 1, 0},
    {0, 1, 1, 0},
    {1, 1, 1, 0},
    {1, 0, 0, 1},
    {0, 1, 0, 1},
    {1, 1, 0, 1},
    {0, 0, 1, 1},
    {1, 0, 1, 1},
    {0, 1, 1, 1},
    {1, 1, 1, 1}
};

std::vector<std::vector<int>> generate_G() {
    std::vector<std::vector<int>> G(11, std::vector<int>(15, 0));
    for (int i = 0; i < 11; ++i) {
        for (int j = 0; j < 4; ++j) {
            G[i][j] = P[i][j];
        }
        G[i][i + 4] = 1;
    }
    return G;
}

std::vector<int> hamming1511_encode(const std::vector<int>& b_in) {
    std::vector<int> b_res;
    auto G = generate_G(); 

    for (size_t i = 0; i < b_in.size(); i += 11) {
        std::vector<int> b(b_in.begin() + i, b_in.begin() + i + 11);
        std::vector<int> c(15, 0);

        for (int j = 0; j < 15; ++j) {
            for (int k = 0; k < 11; ++k) {
                c[j] ^= (b[k] & G[k][j]);
            }
        }

        b_res.insert(b_res.end(), c.begin(), c.end());
    }

    return b_res;
}

void print_bits(const std::vector<int>& bits) {
    for (size_t i = 0; i < bits.size(); ++i) {
        std::cout << bits[i];
    }
    std::cout << "\n";
}

int main() {
    std::vector<int> input_bits = {
        1,0,1,0,1,1,1,0,0,0,1,
        1,1,0,0,0,1,1,1,1,0,1,
        0,0,1,1,0,1,1,0,0,0,0,
        1,0,1,1,0,1,0,0,1,1,1,
        1,0,1,1,1,1,0,0,1,0,0,
        1,1,0,1,1,0,1,0,0,1,0
    };

    std::cout << "WEJŚCIOWE BITY (66):\n";
    print_bits(input_bits);

    std::vector<int> encoded = hamming1511_encode(input_bits);

    std::cout << "\nZAKODOWANE BITY (90):\n";
    print_bits(encoded);

    return 0;
}
