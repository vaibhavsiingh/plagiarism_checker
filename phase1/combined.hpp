#include <array>
#include <iostream>
#include <span>
#include <vector>
#include <cmath>
// -----------------------------------------------------------------------------

// You are free to add any STL includes above this comment, below the --line--.
// DO NOT add "using namespace std;" or include any other files/libraries.
// Also DO NOT add the include "bits/stdc++.h"

// OPTIONAL: Add your helper functions and data structures here
#include <unordered_map>
#include <algorithm>
struct VectorHash {
    std::size_t operator()(const std::vector<int>& v) const {
        std::size_t hash = 0;
        for (int num : v) {
            hash ^= std::hash<int>{}(num) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
        }
        return hash;
    }
};

constexpr int SHORT_PATTERN_MIN = 10;
constexpr int SHORT_PATTERN_MAX = 20;
constexpr int LONG_PATTERN_MIN = 30;
constexpr double APPROX_MATCH_THRESHOLD = 0.8;

 std::array<int, 5> match_submissions(std::vector<int> &tokens1, 
        std::vector<int> &tokens2) {
            int flag = 0;
            int total_exact_length = 0;
            int longest_approx_match_length = 0;
            int start_index_1 = -1;
            int start_index_2 = -1;

            std::unordered_map<std::vector<int>, int, VectorHash> match_indices1;
            std::vector<bool> used_indices1(tokens1.size(), false); // Used indices in tokens1
            std::vector<bool> used_indices2(tokens2.size(), false); // Used indices in tokens2
            int m = tokens1.size(), n = tokens2.size();

            // Function to find exact matches and track them to avoid double counting
            auto find_exact_matches = [&](int min_len, int max_len) {
                for (int len = min_len; len <= max_len; ++len) {
                    for (int i = 0; i <= tokens1.size() - len; ++i) {
                        // Skip if indices are already used to avoid overlaps
                        bool is_used = false;
                        for (int k = i; k < i + len; ++k) {
                            if (used_indices1[k]) {
                                is_used = true;
                                break;
                            }
                        }
                        if (is_used) continue;

                        std::vector<int> seq(tokens1.begin() + i, tokens1.begin() + i + len);
                        match_indices1[seq] = i;
                    }

                    for (int j = 0; j <= tokens2.size() - len; ++j) {
                        // Skip if indices are already used
                        bool is_used = false;
                        for (int k = j; k < j + len; ++k) {
                            if (used_indices2[k]) {
                                is_used = true;
                                break;
                            }
                        }
                        if (is_used) continue;

                        std::vector<int> seq(tokens2.begin() + j, tokens2.begin() + j + len);

                        if (match_indices1.count(seq)) {
                            int i = match_indices1[seq];  // Start index in tokens1

                            // Mark indices as used to avoid overlap
                            for (int k = i; k < i + len; ++k) used_indices1[k] = true;
                            for (int k = j; k < j + len; ++k) used_indices2[k] = true;

                            // Count match length
                            total_exact_length += len;
                        }
                    }
                    match_indices1.clear();
                }
            };

            find_exact_matches(10, 20);
            

            // Approximate Matching for Long Patterns with dynamic length
    std::vector<std::vector<std::pair<int, int>>> dp(m + 1, std::vector<std::pair<int, int>>(n + 1, {0, 0}));
    
    int maxLength = 0;
    int endPos = 0, endPos2 = 0;

    // Fill the DP table
    for (int i = 1; i <= m; i++) {
        for (int j = 1; j <= n; j++) {
            if (tokens1[i - 1] == tokens2[j - 1]) {
                // Match found
                dp[i][j].first = dp[i - 1][j - 1].first + 1;     // Increment length
                dp[i][j].second = dp[i - 1][j - 1].second;       // No additional mismatch
            } else {
                // Mismatch
                dp[i][j].first = dp[i - 1][j - 1].first + 1;     // Increment length
                dp[i][j].second = dp[i - 1][j - 1].second + 1;   // Increment mismatch count
            }
            
            // Check if mismatch ratio is above 20%
            if (dp[i][j].second > 0.1 * dp[i][j].first) {
                // Reset if too many mismatches
                dp[i][j] = {1, (tokens1[i - 1] != tokens2[j - 1]) ? 1 : 0};
            } else if (dp[i][j].first > maxLength) {
                // Update the maximum length and position if current is the longest valid subsequence
                maxLength = dp[i][j].first;
                endPos = i - 1;
                endPos2 = j - 1;
            }
        }   
    }
    
    // Calculate start position of the longest subsequence
    longest_approx_match_length= maxLength;  // Length of the longest fuzzy matching subsequence
    start_index_1 = endPos - maxLength + 1; // Start index in tokens1
    start_index_2 = endPos2 - maxLength + 1; 


            return {flag, total_exact_length, longest_approx_match_length, start_index_1, start_index_2};
        }
