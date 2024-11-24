#include <array>
#include <iostream>
#include <span>
#include <vector>
#include <cmath>
// -----------------------------------------------------------------------------
#include <unordered_map>
#include <algorithm>
#include<unordered_set>
// You are free to add any STL includes above this comment, below the --line--.
// DO NOT add "using namespace std;" or include any other files/libraries.
// Also DO NOT add the include "bits/stdc++.h"

// OPTIONAL: Add your helper functions and data structures here

//approx matching part
const double MAX_MISMATCH_RATIO = 0.2; // 20% mismatch tolerance
const int MIN_LENGTH = 30;  // Minimum length requirement

// Function to find the longest common subsequence with a maximum mismatch ratio
// tokens1: First array
// tokens2: Second array
// flag: 1 for forward, 2 for reverse
// Returns an array of 3 integers: {length, start position in tokens1, start position in tokens2}

std::array<int,3> approximate_searching(std::vector<int> &tokens1, std::vector<int> &tokens2, int flag){  
    
    int m = tokens1.size();
    int n = tokens2.size();
    
    // lcs_table table to store pairs of (length, mismatches)
    std::vector<std::vector<std::pair<int, int>>> lcs_table(m + 1, std::vector<std::pair<int, int>>(n + 1, {0, 0}));
    
    int maxLength = 0;
    int startPos1 = 0;
    int startPos2 = 0;

    for (int i = 1; i <= m; i++) {
        for (int j = 1; j <= n; j++) {
            if (tokens1[i - 1] == tokens2[j - 1]) {
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
                lcs_table[i][j] = {1, (tokens1[i - 1] != tokens2[j - 1]) ? 1 : 0};
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
std::array<int, 3> approximate_match(std::vector<int> &tokens1, std::vector<int> &tokens2) {


    if (tokens1.empty() || tokens2.empty()) {
        return {0, 0, 0};
    }

    // Find the longest common subsequence in both directions    
    std::array<int, 3> x = approximate_searching(tokens1, tokens2,1);

    std::reverse(tokens1.begin(), tokens1.end());
    std::reverse(tokens2.begin(), tokens2.end());

    std::array<int, 3> y = approximate_searching(tokens1, tokens2,2);

    std::reverse(tokens1.begin(), tokens1.end());
    std::reverse(tokens2.begin(), tokens2.end());

    int m = tokens1.size();
    int n = tokens2.size();

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
        if(tokens1[ptr1] != tokens2[ptr2]){
            count_misses++;
        }
        ptr1++;
        ptr2++;
    }


    // move the pointers to the left and right

    int ptr_left1 = left1, ptr_right1 = right1, ptr_left2 = left2, ptr_right2 = right2;
    float curr_ratio = (float)count_misses/(right1-left1+1);
    while(ptr_left1 >= extreme_left1 && ptr_right1 <= extreme_right1){
        if(tokens1[ptr_left1] != tokens2[ptr_left2]){
            count_misses++;
            ptr_right2++;
            ptr_right1++;
        }
        if(tokens1[ptr_right1] != tokens2[ptr_right2]){
            count_misses++;
            ptr_left1--;      
            ptr_left2--;
        }
        if(tokens1[ptr_left1] == tokens2[ptr_left2] && tokens1[ptr_right1] == tokens2[ptr_right2]){
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

//exact length
int computeHash(const std::vector<int>& vec, int start, int length, int base, int mod) {
    std::size_t hash = 0;
    for (int i = 0; i < length; ++i) {
        hash = (hash * base + vec[start + i]) % mod;
    }
    return hash;
}

// Function to update hash in constant time for rolling hash
int rollHash(std::size_t oldHash, int oldVal, int newVal, int base, int power, int mod) {
    //-- Cast `oldVal * power` to std::size_t to prevent overflow
    oldHash = (oldHash + mod - static_cast<std::size_t>(oldVal) * power % mod) % mod;
    oldHash = (oldHash * base + newVal) % mod;
    return oldHash;
}

int findExactMatchLength(const std::vector<int>& tokens1, const std::vector<int>& tokens2) {
    int totalLength = 0;
    std::unordered_set<int> matchedIndices1, matchedIndices2;
    const int base = 31;     // base for hashing
    const int mod = 1e9 + 7; // A large prime modulus to reduce collisions

    for (int length = 20; length >= 10; --length) {
        std::unordered_map<std::size_t, std::vector<int>> hashMap1;

        // Precompute the power for rolling hash update
        std::size_t power = 1;
        for (int i = 0; i < length - 1; ++i) {
            power = (power * base) % mod;
        }

        // Compute hashes for all substrings of `tokens1` of current length
        for (int i = 0; i <= tokens1.size() - length; ++i) {
            if (matchedIndices1.count(i) > 0) continue; // Skip if already matched
            std::size_t hash = computeHash(tokens1, i, length, base, mod);
            hashMap1[hash].push_back(i);
        }

        // Compute rolling hashes for `tokens2` and check for matches
        std::size_t hash = computeHash(tokens2, 0, length, base, mod);
        for (int j = 0; j <= tokens2.size() - length; ++j) {
            if (j > 0) {
                hash = rollHash(hash, tokens2[j - 1], tokens2[j + length - 1], base, power, mod);
            }
            if (matchedIndices2.count(j) > 0) continue; // Skip if already matched

            if (hashMap1.count(hash)) {
                // Possible match found; confirm by direct comparison
                for (int i : hashMap1[hash]) {
                    if (std::equal(tokens1.begin() + i, tokens1.begin() + i + length, tokens2.begin() + j)) {
         
                        totalLength += length;

                        // Mark matched indices to prevent overlaps
                        for (int k = 0; k < length; ++k) {
                            matchedIndices1.insert(i + k);
                            matchedIndices2.insert(j + k);
                        }
                        break; // Stop further matches for this hash to avoid overlap
                    }
                }
            }
        }
    }

    return totalLength;
}

 std::array<int, 5> match_submissions(std::vector<int> &tokens1, 
        std::vector<int> &tokens2) {
            //std::cout<<tokens1.size()<<" "<<tokens2.size()<<std::endl;
            int flag = 0;
            int total_exact_length = 0;
            
            total_exact_length=findExactMatchLength(tokens1,tokens2);

            std::array<int, 3> p=approximate_match(tokens1,tokens2);

            std::cout << tokens1.size() << ' ' << tokens2.size() << std::endl;
            int largertoken=std::max(tokens1.size(),tokens2.size());
            if((total_exact_length*1.0)/(largertoken*1.0)>0.4){
                flag=1;

            }
            else if((p[0]*1.0)/(largertoken*1.0)>0.5){
                flag=1;
            }
            else{
                flag=0;
            }
            return {flag,total_exact_length,p[0],p[1],p[2]};
        }
        