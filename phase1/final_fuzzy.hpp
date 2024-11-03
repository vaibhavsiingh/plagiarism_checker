#include <array>
#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>


const double MAX_MISMATCH_RATIO = 0.2; // 20% mismatch tolerance
const int MIN_LENGTH = 30;  // Minimum length requirement

// Function to find the longest common subsequence with a maximum mismatch ratio
// arr1: First array
// arr2: Second array
// flag: 1 for forward, 2 for reverse
// Returns an array of 3 integers: {length, start position in arr1, start position in arr2}

std::array<int,3> approximate_searching(std::vector<int> &arr1, std::vector<int> &arr2, int flag){
    const int MIN_LENGTH = 30;  
    
    int m = arr1.size();
    int n = arr2.size();
    
    // lcs_table table to store pairs of (length, mismatches)
    std::vector<std::vector<std::pair<int, int>>> lcs_table(m + 1, std::vector<std::pair<int, int>>(n + 1, {0, 0}));
    
    int maxLength = 0;
    int startPos1 = 0;
    int startPos2 = 0;

    for (int i = 1; i <= m; i++) {
        for (int j = 1; j <= n; j++) {
            if (arr1[i - 1] == arr2[j - 1]) {
                // Match found
                lcs_table[i][j].first = lcs_table[i - 1][j - 1].first + 1;
                lcs_table[i][j].second = lcs_table[i - 1][j - 1].second;
            } else {
                lcs_table[i][j].first = lcs_table[i - 1][j - 1].first + 1;
                lcs_table[i][j].second = lcs_table[i - 1][j - 1].second + 1;
            }
            
            // Check if mismatch ratio is within range
            if (lcs_table[i][j].second > MAX_MISMATCH_RATIO * lcs_table[i][j].first) {
                // Too many mismatches, reset sequence
                lcs_table[i][j] = {1, (arr1[i - 1] != arr2[j - 1]) ? 1 : 0};
            } else if (lcs_table[i][j].first >= MIN_LENGTH && lcs_table[i][j].first > maxLength) {
                // Updte maximum length and positions if sequence is long enough
                maxLength = lcs_table[i][j].first;
                if(flag == 1){  
                    startPos1 = i - lcs_table[i][j].first;
                    startPos2 = j - lcs_table[i][j].first;
                }
                else{
                    startPos1 = i-1;
                    startPos2 = j-1;
                }
            }
        }
    }
    
    // Only return non-zero values if we found a sequence of sufficient length
    if (maxLength >= MIN_LENGTH) {
        return { maxLength, startPos1, startPos2};
    }
    
    return { 0, 0, 0};
}

// Function to find the longest common subsequence with a maximum mismatch ratio
std::array<int, 3> approximate_match(std::vector<int> &arr1, std::vector<int> &arr2) {
    if (arr1.empty() || arr2.empty()) {
        return {0, 0, 0};
    }

    // Find the longest common subsequence in both directions    
    std::array<int, 3> x = approximate_searching(arr1, arr2,1);

    std::reverse(arr1.begin(), arr1.end());
    std::reverse(arr2.begin(), arr2.end());

    std::array<int, 3> y = approximate_searching(arr1, arr2,2);

    std::reverse(arr1.begin(), arr1.end());
    std::reverse(arr2.begin(), arr2.end());

    int m = arr1.size();
    int n = arr2.size();

    // Combine the results
    int extreme_left1 = m-y[1]-1;
    int extreme_left2 = n-y[2]-1;
    int left1 = x[1];
    int left2 = x[2];

    int extreme_right1 = x[1] + x[0] - 1;
    int extreme_right2 = x[2] + x[0] - 1;
    int right1 = m-(y[1] - y[0])-2;
    int right2 = n-(y[2] - y[0])-2;

    
    // std::cout << "extreme_left1: " << extreme_left1 << std::endl;
    // std::cout << "extreme_left2: " << extreme_left2 << std::endl;
    // std::cout << "left1: " << left1 << std::endl;
    // std::cout << "left2: " << left2 << std::endl;
    // std::cout << "right1: " << right1 << std::endl;
    // std::cout << "right2: " << right2 << std::endl;
    // std::cout << "extreme_right1: " << extreme_right1 << std::endl;
    // std::cout << "extreme_right2: " << extreme_right2 << std::endl;



    // count the number of current misses
    int ptr1 = left1, ptr2 = left2;
    int count_misses = 0;
    while(ptr1 <= right1 && ptr2 <= right2){
        if(arr1[ptr1] != arr2[ptr2]){
            count_misses++;
        }
        ptr1++;
        ptr2++;
    }


    // move the pointers to the left and right

    int ptr_left1 = left1, ptr_right1 = right1, ptr_left2 = left2, ptr_right2 = right2;
    float curr_ratio = (float)count_misses/(right1-left1+1);
    while(ptr_left1 >= extreme_left1 && ptr_right1 <= extreme_right1){
        if(arr1[ptr_left1] != arr2[ptr_left2]){
            count_misses++;
            ptr_right2++;
            ptr_right1++;
        }
        if(arr1[ptr_right1] != arr2[ptr_right2]){
            count_misses++;
            ptr_left1--;      
            ptr_left2--;
        }
        if(arr1[ptr_left1] == arr2[ptr_left2] && arr1[ptr_right1] == arr2[ptr_right2]){
            ptr_right2++;
            ptr_right1++;
            ptr_left1--;       
            ptr_left2--;
        }
        
        float new_ratio = (float)count_misses/(ptr_right1-ptr_left1+1);
        
        if(new_ratio >= MAX_MISMATCH_RATIO){
            break;
        } 
    }

    // new results
    left1 = ptr_left1+1;
    left2 = ptr_left2+1;
    right1 = ptr_right1-1;
    right2 = ptr_right2-1;

    
    return {right1-left1+1,left1,left2};
}