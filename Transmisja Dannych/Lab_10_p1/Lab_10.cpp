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

    std::vector<int> encoded = hamming74_encode(input_bits);

    std::cout << "BITY:\n";
    print_bits(input_bits);

    std::cout << "\n";

    std::cout << "ZAKODOWANE BITY:\n";
    print_bits(encoded);

    return 0;
}
