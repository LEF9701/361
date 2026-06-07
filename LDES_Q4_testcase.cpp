// LDES -- a toy "Linear DES": a 2-round Feistel cipher on a 4-bit block
// with a 2-bit key K = (k1, k2). The DES S-box is replaced by a linear map.
//
// Encrypting M = 0011 with K = 01 should yield C = 0100.

#include <iostream>
#include <string>

// ---- small bit helpers -------------------------------------------------

// XOR two equal-length bit strings, char by char.
std::string xorBits(const std::string& a, const std::string& b) {
    std::string out(a.size(), '0');
    for (std::size_t i = 0; i < a.size(); ++i)
        out[i] = (a[i] == b[i]) ? '0' : '1';
    return out;
}

// Flip a single bit ('0' <-> '1'). This is the "+1 (mod 2)" from the diagram.
char flip(char bit) { return bit == '0' ? '1' : '0'; }

// Circular shifts by one position.
std::string rotateLeft(const std::string& s)  { return s.substr(1) + s.front(); }
std::string rotateRight(const std::string& s) { return s.back() + s.substr(0, s.size() - 1); }

// ---- the round function f(X, Y) = Z ------------------------------------
// X is the 2-bit right half; keyBit is k1 or k2 (the subkey is that bit x3).
std::string roundFunction(const std::string& X, char keyBit) {
    // Expansion E:  X1 X2  ->  X1 X2 X1
    std::string expanded = { X[0], X[1], X[0] };

    // Subkey: K1 = (k1 k1 k1) or K2 = (k2 k2 k2)
    std::string subkey(3, keyBit);

    // I = E(X) XOR subkey
    std::string I = xorBits(expanded, subkey);

    // Linear map:  J1 = I2 + 1,  J2 = I3 + 1   (I1 is unused)
    std::string J = { flip(I[1]), flip(I[2]) };

    // rotate left -> Z
    return rotateLeft(J);
}

// One Feistel round, in place:  A' = B,  B' = A XOR f(B, keyBit)
void feistelRound(std::string& A, std::string& B, char keyBit) {
    std::string newB = xorBits(A, roundFunction(B, keyBit));
    A = B;
    B = newB;
}

int main() {
    const std::string M = "0011";   // 4-bit plaintext
    const std::string K = "01";     // 2-bit key (k1, k2)

    // Initial permutation: rotate left, then split into halves.
    std::string permuted = rotateLeft(M);
    std::string A = permuted.substr(0, 2);
    std::string B = permuted.substr(2, 2);
    std::cout << "A0 B0: " << A << ' ' << B << '\n';

    // Round 1 uses k1, round 2 uses k2.
    feistelRound(A, B, K[0]);
    std::cout << "A1 B1: " << A << ' ' << B << '\n';

    feistelRound(A, B, K[1]);
    std::cout << "A2 B2: " << A << ' ' << B << '\n';

    // Swap the halves (B2 | A2), then rotate right -> ciphertext.
    std::string C = rotateRight(B + A);
    std::cout << "Ciphertext C: " << C << '\n';

    return 0;
}
