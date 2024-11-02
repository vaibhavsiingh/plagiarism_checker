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
            for (int i = 0; i < tokens1.size(); ++i) {
                for (int j = 0; j < tokens2.size(); ++j) {
                    int length = 0;
                    int match_count = 0;

                    // Extend matching window until similarity drops below threshold
                    while (i + length < tokens1.size() && j + length < tokens2.size()) {
                        if (tokens1[i + length] == tokens2[j + length]) {
                            ++match_count;
                        }
                        ++length;

                        // Check similarity ratio
                        double match_ratio = static_cast<double>(match_count) / length;
                        if (length >= LONG_PATTERN_MIN && match_ratio >= APPROX_MATCH_THRESHOLD) {
                            if (length > longest_approx_match_length) {
                                longest_approx_match_length = length;
                                start_index_1 = i;
                                start_index_2 = j;
                            }
                        } else if (length >= LONG_PATTERN_MIN && match_ratio < APPROX_MATCH_THRESHOLD) {
                            // Stop extending the match if similarity drops below threshold after the minimum length
                            break;
                        }
                    }
                }
            }

            return {flag, total_exact_length, longest_approx_match_length, start_index_1, start_index_2};
        }
