// Caesar Cipher - Exhaustive Key Search (Brute Force) Attack
// -----------------------------------------------------------
// The Caesar cipher has only 26 possible keys (shifts 0..25),
// so an attacker can simply try them all. This program decrypts
// the ciphertext with every key, scores each candidate against
// English letter-frequency, and reports the most likely plaintext.
//
// Compile: g++ -std=c++17 -O2 caesar_brute_force.cpp -o caesar
// Run:     ./caesar

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <cctype>
#include <iomanip>

// Decrypt ciphertext by shifting each letter back by `key` positions.
// Non-alphabetic characters are left unchanged. Case is preserved.
std::string caesarDecrypt(const std::string& ciphertext, int key) {
    std::string plaintext;
    plaintext.reserve(ciphertext.size());

    for (char c : ciphertext) {
        unsigned char uc = static_cast<unsigned char>(c);
        if (std::isupper(uc)) {
            plaintext += static_cast<char>((c - 'A' - key + 26) % 26 + 'A');
        } else if (std::islower(uc)) {
            plaintext += static_cast<char>((c - 'a' - key + 26) % 26 + 'a');
        } else {
            plaintext += c;
        }
    }
    return plaintext;
}

// Score a candidate plaintext using a chi-squared statistic
// against expected English letter frequencies.
// Lower score => closer to the English distribution => more likely plaintext.
double englishScore(const std::string& text) {
    // Expected frequencies of A..Z in English text (percent)
    static const double expectedFreq[26] = {
        8.167, 1.492, 2.782, 4.253, 12.702, 2.228, 2.015,
        6.094, 6.966, 0.153, 0.772, 4.025, 2.406, 6.749,
        7.507, 1.929, 0.095, 5.987, 6.327, 9.056, 2.758,
        0.978, 2.360, 0.150, 1.974, 0.074
    };

    int counts[26] = {0};
    int totalLetters = 0;

    for (char c : text) {
        unsigned char uc = static_cast<unsigned char>(c);
        if (std::isalpha(uc)) {
            counts[std::tolower(uc) - 'a']++;
            totalLetters++;
        }
    }

    if (totalLetters == 0) return 1e9;  // no letters -> worst score

    double chi2 = 0.0;
    for (int i = 0; i < 26; ++i) {
        double expected = expectedFreq[i] * totalLetters / 100.0;
        double observed = counts[i];
        double diff = observed - expected;
        chi2 += (diff * diff) / expected;
    }
    return chi2;
}

int main() {
    std::cout << "Caesar Cipher - Exhaustive Key Search (Brute Force)\n";
    std::cout << "===================================================\n\n";
    std::cout << "Enter ciphertext: ";

    std::string ciphertext;
    std::getline(std::cin, ciphertext);

    if (ciphertext.empty()) {
        std::cerr << "Error: no ciphertext provided.\n";
        return 1;
    }

    std::cout << "\nTrying all 26 possible keys:\n";
    std::cout << "----------------------------------------------------\n";
    std::cout << "Key | Chi^2 Score |  Candidate Plaintext\n";
    std::cout << "----------------------------------------------------\n";

    std::vector<std::pair<double, int>> ranking;  // (score, key)
    ranking.reserve(26);

    for (int key = 0; key < 26; ++key) {
        std::string candidate = caesarDecrypt(ciphertext, key);
        double score = englishScore(candidate);
        ranking.emplace_back(score, key);

        std::cout << std::setw(3) << key << " | "
                  << std::setw(11) << std::fixed << std::setprecision(2) << score
                  << " | " << candidate << "\n";
    }

    // Sort ascending by chi-squared score
    std::sort(ranking.begin(), ranking.end());

    std::cout << "\n====================================================\n";
    std::cout << "Most likely key:       " << ranking[0].second << "\n";
    std::cout << "Most likely plaintext: "
              << caesarDecrypt(ciphertext, ranking[0].second) << "\n";

    std::cout << "\nTop 3 candidates:\n";
    for (int i = 0; i < 3 && i < (int)ranking.size(); ++i) {
        std::cout << "  " << (i + 1) << ". Key=" << ranking[i].second
                  << " (score " << ranking[i].first << ") -> "
                  << caesarDecrypt(ciphertext, ranking[i].second) << "\n";
    }

    return 0;
}
