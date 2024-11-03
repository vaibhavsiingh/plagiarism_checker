#include <array>
#include <iostream>
#include <vector>
#include <cmath>

std::array<int, 5> match_submissions(std::vector<int> &arr1, std::vector<int> &arr2) {
    if (arr1.empty() || arr2.empty()) {
        return {0, 0, 0, 0, 0};
    }
    
    const int MIN_LENGTH = 30;  // Minimum length requirement
    const double MAX_MISMATCH_RATIO = 0.2;  // 20% mismatch tolerance
    
    int m = arr1.size();
    int n = arr2.size();
    
    // DP table to store pairs of (length, mismatches)
    std::vector<std::vector<std::pair<int, int>>> dp(m + 1, std::vector<std::pair<int, int>>(n + 1, {0, 0}));
    
    int maxLength = 0;
    int startPos1 = 0;
    int startPos2 = 0;

    // Fill the DP table
    for (int i = 1; i <= m; i++) {
        for (int j = 1; j <= n; j++) {
            if (arr1[i - 1] == arr2[j - 1]) {
                // Match found
                dp[i][j].first = dp[i - 1][j - 1].first + 1;
                dp[i][j].second = dp[i - 1][j - 1].second;
            } else {
                // Mismatch found
                dp[i][j].first = dp[i - 1][j - 1].first + 1;
                dp[i][j].second = dp[i - 1][j - 1].second + 1;
            }
            
            // Check if mismatch ratio is acceptable
            if (dp[i][j].second > MAX_MISMATCH_RATIO * dp[i][j].first) {
                // Too many mismatches, reset sequence
                dp[i][j] = {1, (arr1[i - 1] != arr2[j - 1]) ? 1 : 0};
            } else if (dp[i][j].first >= MIN_LENGTH && dp[i][j].first > maxLength) {
                // Update maximum length and positions if sequence is long enough
                maxLength = dp[i][j].first;
                startPos1 = i - dp[i][j].first;
                startPos2 = j - dp[i][j].first;
            }
        }
    }
    
    // Only return non-zero values if we found a sequence of sufficient length
    if (maxLength >= MIN_LENGTH) {
        return {0, 0, maxLength, startPos1, startPos2};
    }
    
    return {0, 0, 0, 0, 0};
}