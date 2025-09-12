//Dmytro Mahaliuk 55722



#include <iostream>
#include <vector>



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

void introduce_errors(std::vector<int>& bits) {
    size_t n = bits.size();
    size_t num_blocks = n / 7;

    for (size_t j = 0; j < num_blocks; ++j) {
        size_t idx = j * 7 + (j % 7);
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
        1, 0, 1, 1,   0, 1, 0, 0,   1, 1, 0, 1,   0, 0, 1, 1,   1, 0, 0, 1,
        1, 1, 1, 0,   0, 1, 1, 0,   0, 0, 0, 1,   1, 0, 1, 0,   0, 1, 0, 1
    };

    std::cout << "BITY:\n";
    print_bits(input_bits);

    std::vector<int> encoded_clean = hamming74_encode(input_bits);
    std::vector<int> encoded_damaged = hamming74_encode(input_bits);
    introduce_errors(encoded_damaged);

    std::cout << "\n";

    std::cout << "ZAKODOWANE BITY NIEUSZKODZONY:\n";
    print_bits(encoded_clean);

    std::cout << "\n";

    std::cout << "ZAKODOWANE BITY USZKODZONY:\n";
    print_bits(encoded_damaged);

    std::vector<int> decoded_clean = hamming74_decode(encoded_clean);
    std::vector<int> decoded_damaged = hamming74_decode(encoded_damaged);

    std::cout << "\n";

    std::cout << "ODEKODOWANE NIEUSZKODZONY BITY:\n";
    print_bits(decoded_clean);

    std::cout << "\n";

    std::cout << "ODEKODOWANE USZKODZONY BITY:\n";
    print_bits(decoded_damaged);

    return 0;
}