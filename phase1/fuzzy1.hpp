#include <array>
#include <iostream>
#include <vector>
#include<cmath>

// Function to find the longest fuzzy matching subsequence
std::array<int, 5> match_submissions(std::vector<int> &arr1, std::vector<int> &arr2) {
    if (arr1.empty() || arr2.empty()) {
        return {0, 0, 0, 0, 0};
    }
    
    std::array<int, 5> result = {0, 0, 0, 0, 0};  // Result array
    
    int m = arr1.size();
    int n = arr2.size();
    
    // DP table to store pairs of (length, mismatches)
    std::vector<std::vector<std::pair<int, int>>> dp(m + 1, std::vector<std::pair<int, int>>(n + 1, {0, 0}));
    
    int maxLength = 0;
    int endPos = 0;

    // Fill the DP table
    for (int i = 1; i <= m; i++) {
        for (int j = 1; j <= n; j++) {
            if (arr1[i - 1] == arr2[j - 1]) {
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
                dp[i][j] = {0, 0};
            } else if (dp[i][j].first > maxLength) {
                // Update the maximum length and position if current is the longest valid subsequence
                maxLength = dp[i][j].first;
                endPos = i - 1;
            }
        }
    }
    
    // Calculate start position of the longest subsequence
    result[2] = maxLength;  // Length of the longest fuzzy matching subsequence
    result[3] = endPos - maxLength + 1; // Start index in arr1

    // Display the result for verification
    // for (int i = 0; i < 5; i++) {
    //     std::cout << result[i] << ' ';
    // }
    // std::cout << "\n";

    // for(int i=0; i<m; i++){
    //     std::cout << arr1[i] << " ";
    // }
    // std::cout << "\n\n\n\n";
    // for(int i=0; i<n; i++){
    //     std::cout << arr2[i] << " ";
    // }
    
    return result;
}
