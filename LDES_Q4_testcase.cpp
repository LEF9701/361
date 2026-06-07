// LDES -- a 2-round Feistel cipher on a 4-bit block with a 2-bit key K=(k1,k2).
// The DES S-box is replaced by a linear map.
//
// Because it is a Feistel network with a symmetric wrapper (rotateLeft at the
// start, swap + rotateRight at the end), the SAME routine encrypts and
// decrypts -- decryption just feeds the round keys in reverse order:
//
//     C = crypt(M, k1, k2);   // encrypt
//     M = crypt(C, k2, k1);   // decrypt

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
// X is the 2-bit half; keyBit is k1 or k2 (the subkey is that bit x3).
std::string roundFunction(const std::string& X, char keyBit) {
    std::string expanded = { X[0], X[1], X[0] };   // E:  X1 X2 -> X1 X2 X1
    std::string subkey(3, keyBit);                 // K1=(k1 k1 k1) or K2=(k2 k2 k2)
    std::string I = xorBits(expanded, subkey);     // I = E(X) XOR subkey
    std::string J = { flip(I[1]), flip(I[2]) };    // J1 = I2+1, J2 = I3+1
    return rotateLeft(J);                          // rotate left -> Z
}

// One Feistel round, in place:  A' = B,  B' = A XOR f(B, keyBit)
void feistelRound(std::string& A, std::string& B, char keyBit) {
    std::string newB = xorBits(A, roundFunction(B, keyBit));
    A = B;
    B = newB;
}

// Core routine. Encryption feeds keys in order (k1, k2);
// decryption feeds them reversed (k2, k1).
std::string crypt(const std::string& in, char firstKey, char secondKey) {
    std::string permuted = rotateLeft(in);          // initial permutation
    std::string A = permuted.substr(0, 2);
    std::string B = permuted.substr(2, 2);

    feistelRound(A, B, firstKey);                   // round 1
    feistelRound(A, B, secondKey);                  // round 2

    return rotateRight(B + A);                      // swap halves, then rotate right
}

int main() {
    const std::string M = "0011";   // 4-bit plaintext
    const std::string K = "01";     // 2-bit key (k1, k2)

    std::string C         = crypt(M, K[0], K[1]);   // encrypt
    std::string recovered = crypt(C, K[1], K[0]);   // decrypt (keys reversed)

    std::cout << "Plaintext  M : " << M << '\n';
    std::cout << "Key        K : " << K << '\n';
    std::cout << "Ciphertext C : " << C << '\n';
    std::cout << "Decrypted    : " << recovered
              << (recovered == M ? "   (matches M)" : "   (MISMATCH!)") << '\n';

    return 0;
}
