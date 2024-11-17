#include "structures.hpp"
// -----------------------------------------------------------------------------

// You are free to add any STL includes above this comment, below the --line--.
// DO NOT add "using namespace std;" or include any other files/libraries.
// Also DO NOT add the include "bits/stdc++.h"

// OPTIONAL: Add your helper functions and classes here

class plagiarism_checker_t {
    // You should NOT modify the public interface of this class.
public:
    plagiarism_checker_t(void);
    plagiarism_checker_t(std::vector<std::shared_ptr<submission_t>> 
                            __submissions);
    ~plagiarism_checker_t(void);
    void add_submission(std::shared_ptr<submission_t> __submission);

protected:
    // TODO: Add members and function signatures here
    const int MOD = 1e9 + 7;  // Large prime for modulo
    const int BASE = 31;      // Base for hashing

    // Function to compute rolling hashes for a sequence with subarray length L
    vector<pair<int, int>> computeHashes(const vector<int>& seq, int L) {
        vector<pair<int, int>> hashes;  // {hash value, start index}
        if (seq.size() < L) return hashes;

        long long hash = 0, power = 1;
        // Initial Hash
        for (int i = 0; i < L; i++) {
            hash = (hash * BASE + seq[i]) % MOD;
            if (i < L - 1){
                power = (power * BASE) % MOD;
            }
        }
        hashes.push_back({hash, 0});

        // Rolling Hash constant time
        for (int i = L; i < seq.size(); i++) {
            hash = (hash * BASE - seq[i - L] * power % MOD + MOD) % MOD; 
            hash = (hash + seq[i]) % MOD;                               
            hashes.push_back({hash, i - L + 1});
        }
        return hashes;
    }

    bool hasMatchingPair(const vector<vector<int>>& sequences) {
        unordered_map<int, vector<pair<int, int>>> hashTable;  // {hash,(seq,start)}
        int L=74;
        std::vector<std::pair<int,int>>matches;
        for (int seq = 0; seq < sequences.size(); seq++) {
            vector<pair<int, int>> hashes = computeHashes(sequences[seq], L);

            for (auto& [hash, start_idx] : hashes) {
                if (hashTable.find(hash) != hashTable.end()) {
                    for (auto& [other_seq, other_start_idx] : hashTable[hash]) {
                        if (other_seq != seq) {
                            // Compare hash values
                            bool match = true;
                            for (int k = 0; k < L; k++) {
                                if (sequences[seq][start_idx + k] !=
                                    sequences[other_seq][other_start_idx + k]) {
                                    match = false;
                                    break;
                                }
                            }
                            if (match){
                                matches.insert{seq,other_seq};
                                break;
                            }
                        }
                    }
                }
                hashTable[hash].emplace_back(seq, start_idx);
            }
        }
        return false;
    }
    // End TODO
};
