#include <array>
#include <vector>
#include <algorithm>

std::array<int, 5> match_submissions(std::vector<int>& arr1, std::vector<int>& arr2) {
    if (arr1.empty() || arr2.empty()) {
        return {0, 0, 0, 0, 0};
    }
    
    const int MIN_LENGTH = 30;  // Minimum sequence length
    const double MAX_DISTANCE_RATIO = 0.2;  // Maximum allowed Levenshtein distance ratio
    
    int m = arr1.size();
    int n = arr2.size();
    
    // DP table to store Levenshtein distances
    std::vector<std::vector<int>> dp(m + 1, std::vector<int>(n + 1, 0));
    
    // Initialize first row and column
    for (int i = 0; i <= m; i++) dp[i][0] = i;
    for (int j = 0; j <= n; j++) dp[0][j] = j;
    
    // Variables to track best match
    int best_length = 0;
    int best_start1 = 0;
    int best_start2 = 0;
    
    // Fill the DP table and track potential endings of matches
    for (int i = 1; i <= m; i++) {
        for (int j = 1; j <= n; j++) {
            if (arr1[i-1] == arr2[j-1]) {
                dp[i][j] = dp[i-1][j-1];
            } else {
                dp[i][j] = 1 + std::min({dp[i-1][j], dp[i][j-1], dp[i-1][j-1]});
            }
            
            // For each position (i,j), check for valid sequences ending here
            // by looking back up to the minimum required length
            for (int len = MIN_LENGTH; len <= std::min({i, j, best_length + 1}); len++) {
                // Get Levenshtein distance for this sequence
                int distance = dp[i][j] - dp[i-len][j-len];
                
                // Check if this is a valid match
                if (distance <= len * MAX_DISTANCE_RATIO) {
                    if (len > best_length) {
                        best_length = len;
                        best_start1 = i - len;
                        best_start2 = j - len;
                    }
                    // No need to check smaller lengths from this endpoint
                    break;
                }
            }
        }
    }
    
    // If no match found of sufficient length
    if (best_length < MIN_LENGTH) {
        return {0, 0, 0, 0, 0};
    }
    
    return {0, 0, best_length, best_start1, best_start2};
}