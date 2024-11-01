#include <array>
#include <iostream>
#include <span>
#include <vector>
#include<cmath>



    int findPatternMatchesLength(const std::vector<int>& arr1, const std::vector<int>& arr2,
                               int minLength = 10, int maxLength = 20) {
        if (arr1.empty() || arr2.empty()) {
            return 0;
        }
        
        int m = arr1.size();
        int n = arr2.size();
        std::vector<std::vector<int>> dp(m + 1, std::vector<int>(n + 1, 0));
        
        int totalLength = 0;
        
        // Fill the DP table and count pattern lengths
        for (int i = 1; i <= m; i++) {
            for (int j = 1; j <= n; j++) {
                if (arr1[i-1] == arr2[j-1]) {
                    dp[i][j] = dp[i-1][j-1] + 1;
                    
                    // If we've found a match within our desired length range
                    if (dp[i][j] >= minLength) {
                        int currentLength = dp[i][j];
                        
                        // Break longer matches into segments and add their lengths
                        while (currentLength >= minLength) {
                            int segmentLength = std::min(currentLength, maxLength);
                            totalLength += segmentLength;
                            currentLength -= maxLength;
                        }
                    }
                }
            }
        }
        
        return totalLength;
    }


    std::array<int, 5> match_submissions(std::vector<int> &arr1, 
        std::vector<int> &arr2) {
    // TODO: Write your code here
        if (arr1.empty() || arr2.empty()) {
            return {0};
        }
        int minLength = 10, maxLength = 20;
        std::array<int, 5> result = {0, 0, 0, 0, 0};

        int m = arr1.size();
        int n = arr2.size();
        std::vector<std::vector<int>> dp(m + 1, std::vector<int>(n + 1, 0));
        
        int totalLength = 0;
        
        // Fill the DP table and count pattern lengths
        for (int i = 1; i <= m; i++) {
            for (int j = 1; j <= n; j++) {
                if (arr1[i-1] == arr2[j-1]) {
                    dp[i][j] = dp[i-1][j-1] + 1;
                    
                    // If we've found a match within our desired length range
                    if (dp[i][j] >= minLength) {
                        int currentLength = dp[i][j];
                        
                        // Break longer matches into segments and add their lengths
                        while (currentLength >= minLength) {
                            int segmentLength = std::min(currentLength, maxLength);
                            totalLength += segmentLength;
                            currentLength -= maxLength;
                        }
                    }
                }
            }
        }

        result[1] = totalLength;
        return result;


    }