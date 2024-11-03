#include <array>
#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>

std::array<int,3> fuzzy_match(std::vector<int> &arr1, std::vector<int> &arr2){
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
        return { maxLength, startPos1, startPos2};
    }
    
    return { 0, 0, 0};
}




std::array<int, 5> match_submissions(std::vector<int> &arr1, std::vector<int> &arr2) {
    if (arr1.empty() || arr2.empty()) {
        return {0, 0, 0, 0, 0};
    }
    
    const int MIN_LENGTH = 30;  // Minimum length requirement
    const double MAX_MISMATCH_RATIO = 0.2;  // 20% mismatch tolerance
    
//  auto x = fuzzy_match(arr1, arr2);

    // if (x[0] >= MIN_LENGTH) {
    //     return {0, 0, x[0], x[1], x[2]};
    // }

    std::reverse(arr1.begin(), arr1.end());
    std::reverse(arr2.begin(), arr2.end());

    auto y = fuzzy_match(arr1, arr2);

    if (y[0] >= MIN_LENGTH) {
        int n = arr1.size(), m = arr2.size();
        std::cout << "n: " << n << " m: " << m << std::endl;
        return {0, 0, y[0], y[1]+y[0], y[2]+y[0]};
    }


    
    return {0, 0, 0, 0, 0};
}