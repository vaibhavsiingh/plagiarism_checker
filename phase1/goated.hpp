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

            std::unordered_map<size_t, std::vector<int>> hash_map;

            // Helper function to compute a simple hash for a sequence
            auto compute_hash = [](const std::vector<int>& tokens, int start, int length) -> size_t {
                size_t hash = 0;
                for (int i = 0; i < length; ++i) {
                    hash = hash * 31 + tokens[start + i];
                }
                return hash;
            };

            // Exact Matching for Short Patterns
            for (int length = SHORT_PATTERN_MIN; length <= SHORT_PATTERN_MAX; ++length) {
                for (int i = 0; i <= tokens1.size() - length; ++i) {
                    size_t hash = compute_hash(tokens1, i, length);
                    hash_map[hash].push_back(i);
                }

                for (int j = 0; j <= tokens2.size() - length; ++j) {
                    size_t hash = compute_hash(tokens2, j, length);
                    if (hash_map.find(hash) != hash_map.end()) {
                        total_exact_length += length;
                        flag = 1;
                        break;
                    }
                }
                hash_map.clear();
            }

            // Approximate Matching for Long Patterns
            for (int i = 0; i <= tokens1.size() - LONG_PATTERN_MIN; ++i) {
                for (int j = 0; j <= tokens2.size() - LONG_PATTERN_MIN; ++j) {
                    int length = 0;
                    int match_count = 0;

                    while (i + length < tokens1.size() && j + length < tokens2.size() && length < LONG_PATTERN_MIN) {
                        if (tokens1[i + length] == tokens2[j + length]) {
                            ++match_count;
                        }
                        ++length;
                    }

                    double match_ratio = static_cast<double>(match_count) / length;
                    if (match_ratio >= APPROX_MATCH_THRESHOLD && length > longest_approx_match_length) {
                        longest_approx_match_length = length;
                        start_index_1 = i;
                        start_index_2 = j;
                    }
                }
            }

            return {flag, total_exact_length, longest_approx_match_length, start_index_1, start_index_2};
        }
