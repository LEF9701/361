// rsa_demo.cpp
// A minimal, educational RSA implementation in C++ — no external libraries.
//
// WARNING: For LEARNING ONLY. It uses small numbers and is NOT secure.
// Real RSA needs 2048+ bit keys, cryptographically secure random primes,
// and padding (OAEP). This is "textbook RSA" to show the core idea.
//
// Compile: g++ -std=c++17 -O2 rsa_demo.cpp -o rsa_demo
// Run:     ./rsa_demo

#include <iostream>
#include <cstdint>
#include <string>
#include <vector>

using u64 = uint64_t;

// ---- Modular multiplication: (a * b) % m without overflow ----
// __int128 holds the full 128-bit product before we reduce mod m.
u64 mulmod(u64 a, u64 b, u64 m) {
    return (u64)((__uint128_t)a * b % m);
}

// ---- Modular exponentiation: (base^exp) % m, via square-and-multiply ----
// This is THE key operation in RSA. Naively computing m^e would explode;
// here we keep everything reduced mod m at each step.
u64 powmod(u64 base, u64 exp, u64 m) {
    u64 result = 1 % m;
    base %= m;
    while (exp > 0) {
        if (exp & 1) result = mulmod(result, base, m); // odd bit -> multiply
        base = mulmod(base, base, m);                  // square the base
        exp >>= 1;                                      // next bit
    }
    return result;
}

// ---- Greatest common divisor (Euclid) ----
u64 gcd(u64 a, u64 b) {
    while (b) { u64 t = a % b; a = b; b = t; }
    return a;
}

// ---- Extended Euclidean algorithm: find x such that (a * x) % m == 1 ----
// i.e. the modular inverse of a mod m. Needed to derive the private key d.
int64_t modinv(int64_t a, int64_t m) {
    int64_t m0 = m, x0 = 0, x1 = 1;
    if (m == 1) return 0;
    while (a > 1) {
        int64_t q = a / m;
        int64_t t = m;  m = a % m;  a = t;
        t = x0;  x0 = x1 - q * x0;  x1 = t;
    }
    if (x1 < 0) x1 += m0;
    return x1;
}

// ---- Simple primality test (trial division) — fine for small numbers ----
bool isPrime(u64 n) {
    if (n < 2) return false;
    for (u64 i = 2; i * i <= n; i++)
        if (n % i == 0) return false;
    return true;
}

int main() {
    // 1. Pick two distinct primes. (In real RSA these are huge & random.)
    u64 p = 61, q = 53;            // try changing these!
    if (!isPrime(p) || !isPrime(q) || p == q) {
        std::cerr << "p and q must be distinct primes.\n";
        return 1;
    }

    // 2. Compute the modulus n and Euler's totient phi(n).
    u64 n   = p * q;               // 3233
    u64 phi = (p - 1) * (q - 1);   // 3120

    // 3. Choose public exponent e: 1 < e < phi and gcd(e, phi) == 1.
    u64 e = 17;
    while (gcd(e, phi) != 1) e++;

    // 4. Private exponent d = e^(-1) mod phi.
    u64 d = (u64)modinv((int64_t)e, (int64_t)phi);

    std::cout << "Primes:      p = " << p << ", q = " << q << "\n";
    std::cout << "Public key:  (e, n) = (" << e << ", " << n << ")\n";
    std::cout << "Private key: (d, n) = (" << d << ", " << n << ")\n\n";

    // 5. Encrypt and decrypt a single number (must be < n).
    u64 message = 65;
    u64 cipher  = powmod(message, e, n);   // c = m^e mod n
    u64 back    = powmod(cipher,  d, n);   // m = c^d mod n
    std::cout << "Number demo:  " << message
              << " -> encrypt -> " << cipher
              << " -> decrypt -> " << back << "\n\n";

    // 6. Encrypt a short string, one character at a time.
    //    (Each char's ASCII value is < n, so it works. Insecure, but shows it.)
    std::string text = "HELLO";
    std::vector<u64> enc;
    for (char ch : text)
        enc.push_back(powmod((u64)(unsigned char)ch, e, n));

    std::string dec;
    for (u64 c : enc)
        dec.push_back((char)powmod(c, d, n));

    std::cout << "Text demo:    \"" << text << "\"\n";
    std::cout << "  ciphertext: ";
    for (u64 c : enc) std::cout << c << " ";
    std::cout << "\n  decrypted:  \"" << dec << "\"\n";

    return 0;
}
