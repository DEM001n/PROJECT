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

std::vector<int> hamming1511_encode(const std::vector<int>& b_in) {
    std::vector<int> b_res;

    std::vector<std::vector<int>> G(11, std::vector<int>(15, 0));
    for (int i = 0; i < 11; ++i) {
        for (int j = 0; j < 4; ++j) {
            G[i][j] = P[i][j];
        }
        G[i][i + 4] = 1;
    }

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

std::vector<int> hamming1511_decode(const std::vector<int>& input_bits) {

    int H[4][15] = { 0 };
    for (int i = 0; i < 4; ++i) {
        H[i][i] = 1; 
        for (int j = 0; j < 11; ++j) {
            H[i][j + 4] = P[j][i];
        }
    }

    std::vector<int> result;
    size_t n = input_bits.size();

    for (size_t i = 0; i < n; i += 15) {
        std::vector<int> block(input_bits.begin() + i, input_bits.begin() + i + 15);

        std::vector<int> reordered = {
            block[0], block[1], block[3], block[7],  
            block[2], block[4], block[5], block[6],  
            block[8], block[9], block[10], block[11], block[12], block[13], block[14]  
        };

        int s[4] = { 0 };
        for (int r = 0; r < 4; ++r) {
            for (int c = 0; c < 15; ++c) {
                s[r] ^= reordered[c] * H[r][c];
            }
        }

        int error_index = s[0] + (s[1] << 1) + (s[2] << 2) + (s[3] << 3);

        if (error_index > 0 && error_index <= 15) {
            reordered[error_index - 1] ^= 1;
        }

        result.insert(result.end(), reordered.begin() + 4, reordered.end());
    }

    return result;
}

void introduce_errors(std::vector<int>& bits) {
    size_t n = bits.size();
    size_t num_blocks = n / 15;

    for (size_t j = 0; j < num_blocks; ++j) {
        size_t idx = j * 15 + (j % 15);
        if (idx < n) {
            bits[idx] ^= 1;
        }
    }
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

    std::cout << "WEJŚCIOWE BITY :\n";
    print_bits(input_bits);

    std::vector<int> encoded_clear = hamming1511_encode(input_bits);
    std::vector<int> encoded_damaged = hamming1511_encode(input_bits);
    introduce_errors(encoded_damaged);

    std::cout << "\nZAKODOWANE BITY NIEUSZKODZONY:\n";
    print_bits(encoded_clear);

    std::cout << "ZAKODOWANE BITY USZKODZONY:\n";
    print_bits(encoded_damaged);

    std::vector<int> decoded_clear = hamming1511_decode(encoded_clear);
    std::vector<int> decoded_damaged = hamming1511_decode(encoded_damaged);

    std::cout << "\nODEKODOWANE NIEUSZKODZONY BITY:\n";
    print_bits(decoded_clear);

    std::cout << "ODEKODOWANE USZKODZONY BITY:\n";
    print_bits(decoded_clear);

    return 0;
}

