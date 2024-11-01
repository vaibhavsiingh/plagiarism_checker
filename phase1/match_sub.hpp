#include <array>
#include <iostream>
#include <span>
#include <vector>
#include<cmath>
// -----------------------------------------------------------------------------

// You are free to add any STL includes above this comment, below the --line--.
// DO NOT add "using namespace std;" or include any other files/libraries.
// Also DO NOT add the include "bits/stdc++.h"

// OPTIONAL: Add your helper functions and data structures here
    


std::array<int, 5> match_submissions(std::vector<int> &arr1, 
        std::vector<int> &arr2) {
    // TODO: Write your code here
        if (arr1.empty() || arr2.empty()) {
            return {0};
        }
        std::array<int, 5> result = {0, 0, 0, 0, 0};
        
        int m = arr1.size();
        int n = arr2.size();
        
        // Create DP table
        std::vector<std::vector<int>> dp(m + 1, std::vector<int>(n + 1, 0));
        
        // Variables to track maximum length and ending position
        int maxLength = 0;
        int endPos = 0;
        
        // Fill the DP table
        for (int i = 1; i <= m; i++) {
            for (int j = 1; j <= n; j++) {
                if (arr1[i-1] == arr2[j-1]) {
                    dp[i][j] = dp[i-1][j-1] + 1;
                    if (dp[i][j] > maxLength) {
                        maxLength = dp[i][j];
                        endPos = i - 1;
                    }
                }
            }
        }
        
        // Extract the subarray
        
        result[1] = endPos - maxLength + 1;

        for(int i = 0; i < 5; i++) {
        std::cout << result[i] << ' ';
    }
    std::cout << "\n";
    return result;

    }
     // dummy return
    // End TODO

