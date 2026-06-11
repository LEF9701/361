#include <cstdint>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

uint32_t leftrotate(uint32_t x, uint32_t n) {
    return (x << n) | (x >> (32 - n));
}

std::string sha1(const std::string& input) {
    std::vector<uint8_t> msg(input.begin(), input.end());

    uint64_t original_bit_len = msg.size() * 8;

    // append 1 bit: 10000000
    msg.push_back(0x80);

    // pad with zeros until length ≡ 448 mod 512
    while ((msg.size() * 8) % 512 != 448) {
        msg.push_back(0x00);
    }

    // append original length as 64-bit big-endian
    for (int i = 7; i >= 0; --i) {
        msg.push_back((original_bit_len >> (i * 8)) & 0xff);
    }

    uint32_t h0 = 0x67452301;
    uint32_t h1 = 0xEFCDAB89;
    uint32_t h2 = 0x98BADCFE;
    uint32_t h3 = 0x10325476;
    uint32_t h4 = 0xC3D2E1F0;

    for (size_t chunk = 0; chunk < msg.size(); chunk += 64) {
        uint32_t w[80];

        for (int i = 0; i < 16; ++i) {
            w[i] =
                (msg[chunk + i * 4 + 0] << 24) |
                (msg[chunk + i * 4 + 1] << 16) |
                (msg[chunk + i * 4 + 2] << 8)  |
                (msg[chunk + i * 4 + 3]);
        }

        for (int i = 16; i < 80; ++i) {
            w[i] = leftrotate(w[i - 3] ^ w[i - 8] ^ w[i - 14] ^ w[i - 16], 1);
        }

        uint32_t a = h0;
        uint32_t b = h1;
        uint32_t c = h2;
        uint32_t d = h3;
        uint32_t e = h4;

        for (int i = 0; i < 80; ++i) {
            uint32_t f, k;

            if (i < 20) {
                f = (b & c) | ((~b) & d);
                k = 0x5A827999;
            } else if (i < 40) {
                f = b ^ c ^ d;
                k = 0x6ED9EBA1;
            } else if (i < 60) {
                f = (b & c) | (b & d) | (c & d);
                k = 0x8F1BBCDC;
            } else {
                f = b ^ c ^ d;
                k = 0xCA62C1D6;
            }

            uint32_t temp = leftrotate(a, 5) + f + e + k + w[i];

            e = d;
            d = c;
            c = leftrotate(b, 30);
            b = a;
            a = temp;
        }

        h0 += a;
        h1 += b;
        h2 += c;
        h3 += d;
        h4 += e;
    }

    std::ostringstream out;
    out << std::hex << std::setfill('0');
    out << std::setw(8) << h0;
    out << std::setw(8) << h1;
    out << std::setw(8) << h2;
    out << std::setw(8) << h3;
    out << std::setw(8) << h4;

    return out.str();
}

int main() {
    std::string msg = "abc";

    std::cout << sha1(msg) << '\n';
}
