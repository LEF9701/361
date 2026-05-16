#include <iostream>
#include <string>
#include <vector>
#include <iomanip>
#include <cmath>
#include <map>
#include <algorithm>

using namespace std;

// Remove spaces and convert to uppercase
string preprocessText(const string& text) {
    string result;
    for (char c : text) {
        if (isalpha(c)) {
            result += toupper(c);
        }
    }
    return result;
}

// Calculate frequency of each letter in a substring
map<char, int> getFrequencies(const string& text) {
    map<char, int> freq;
    for (char c : text) {
        freq[c]++;
    }
    return freq;
}

// Calculate Index of Coincidence for a given text
double calculateIC(const string& text) {
    if (text.length() <= 1) return 0.0;
    
    map<char, int> freq = getFrequencies(text);
    double numerator = 0.0;
    int n = text.length();
    
    for (auto& p : freq) {
        int count = p.second;
        numerator += count * (count - 1.0);
    }
    
    double ic = numerator / (n * (n - 1.0));
    return ic;
}

// Extract every nth character starting from position 'start'
string extractEveryNth(const string& text, int period, int start) {
    string result;
    for (int i = start; i < text.length(); i += period) {
        result += text[i];
    }
    return result;
}

// Analyze period by calculating average IC for all residue classes
double analyzePeriod(const string& ciphertext, int period) {
    vector<double> ics;
    
    for (int i = 0; i < period; i++) {
        string subsequence = extractEveryNth(ciphertext, period, i);
        double ic = calculateIC(subsequence);
        ics.push_back(ic);
    }
    
    double avgIC = 0.0;
    for (double ic : ics) {
        avgIC += ic;
    }
    avgIC /= period;
    
    return avgIC;
}

int main() {
    string ciphertext = "EQYNTMJMDY GS TFB KPTFBKPTGES, SUEF PS LUKABQ TFBMQY PLR TFB PNNJGEPTGML "
                       "MC CMQKUJPS PLR PJDMQGTFKS, TFPT ULRBQNGL EQYNTMDQPNFY PLR EQYNTPLPJYSGS. "
                       "EQYNTPLPJYSGS EMLEBNTS PQB FGDFJY SNBEGPJGZBR PLR EMKNJBX, SM TFGS RBCGLGTGML "
                       "EMLEBLTQPTBS ML SMKB MC TFB IBY KPTFBKPTGEPJ EMLEBNTS ABFGLR EQYNTMDQPNFY, "
                       "PS WBJJ PS KMRBQL BXPKNJBS MC GTS USB";
    
    // Preprocess ciphertext
    string processed = preprocessText(ciphertext);
    
    cout << "╔═══════════════════════════════════════════════════════════════════════════════╗" << endl;
    cout << "║                  VIGENÈRE CIPHER PERIOD ANALYSIS                              ║" << endl;
    cout << "║              Using Index of Coincidence (IC) Method                           ║" << endl;
    cout << "╚═══════════════════════════════════════════════════════════════════════════════╝" << endl << endl;
    
    cout << "Ciphertext (processed): " << processed << endl;
    cout << "Length: " << processed.length() << " characters" << endl;
    cout << "Target IC (English): 0.0650" << endl << endl;
    
    cout << "┌──────────────────────────────────────────────────────────────────────────────┐" << endl;
    cout << "│ PERIOD ANALYSIS RESULTS                                                      │" << endl;
    cout << "└──────────────────────────────────────────────────────────────────────────────┘" << endl;
    
    vector<pair<int, double>> results;
    double bestIC = 0.0;
    int bestPeriod = 1;
    double minDistance = 1.0;
    
    for (int period = 1; period <= 8; period++) {
        double avgIC = analyzePeriod(processed, period);
        double distance = fabs(avgIC - 0.065);
        
        results.push_back({period, avgIC});
        
        // Format output
        cout << "\nPeriod " << period << ":" << endl;
        cout << "  IC Value:     " << fixed << setprecision(4) << avgIC << endl;
        cout << "  Distance from 0.0650: " << fixed << setprecision(4) << distance << endl;
        
        // Visual indicator
        cout << "  Status:       ";
        if (distance < 0.010) {
            cout << "★★★ EXCELLENT MATCH ★★★";
        } else if (distance < 0.020) {
            cout << "★★ VERY GOOD ★★";
        } else if (distance < 0.030) {
            cout << "★ GOOD";
        } else {
            cout << "Poor";
        }
        cout << endl;
        
        if (distance < minDistance) {
            minDistance = distance;
            bestIC = avgIC;
            bestPeriod = period;
        }
    }
    
    cout << "\n┌──────────────────────────────────────────────────────────────────────────────┐" << endl;
    cout << "│ FINAL RESULT                                                                 │" << endl;
    cout << "└──────────────────────────────────────────────────────────────────────────────┘" << endl;
    cout << "\n✓ Most Likely Period: " << bestPeriod << endl;
    cout << "  Calculated IC: " << fixed << setprecision(4) << bestIC << endl;
    cout << "  Distance from English IC: " << fixed << setprecision(4) << minDistance << endl;
    
    // Summary table
    cout << "\n┌──────────────────────────────────────────────────────────────────────────────┐" << endl;
    cout << "│ SUMMARY TABLE                                                                │" << endl;
    cout << "├─────────┬────────────┬──────────────┐" << endl;
    cout << "│ Period  │ IC Value   │ Distance     │" << endl;
    cout << "├─────────┼────────────┼──────────────┤" << endl;
    
    for (auto& p : results) {
        double dist = fabs(p.second - 0.065);
        cout << "│ " << setw(5) << p.first << "   │ " 
             << fixed << setprecision(4) << setw(10) << p.second << " │ "
             << fixed << setprecision(4) << setw(10) << dist << "   │" << endl;
    }
    cout << "├─────────┼────────────┼──────────────┤" << endl;
    cout << "│ BEST    │ " << fixed << setprecision(4) << setw(10) << bestIC << " │ "
         << fixed << setprecision(4) << setw(10) << minDistance << "   │" << endl;
    cout << "└─────────┴────────────┴──────────────┘" << endl;
    
    cout << "\n[CRYPTANALYSIS INTERPRETATION]" << endl;
    cout << "The key length (period) of the Vigenère cipher is most likely: " << bestPeriod << endl;
    
    return 0;
}
