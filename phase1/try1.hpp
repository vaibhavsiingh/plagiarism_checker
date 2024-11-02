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

bool checkExactMatch(std::vector<int> &submission1, std::vector<int> &submission2, int start1, int start2, int length){
    for (size_t i = 0; i < length; ++i) {
        if (submission1[start1 + i] != submission2[start2 + i]) {
            return false;
        }
    }
    return true;
}

bool checkApproximateMatch(std::vector<int> &submission1, std::vector<int> &submission2, int start1, int start2, int length){
    size_t matches = 0;
    for (size_t i = 0; i < length; ++i) {
        if (submission1[start1 + i] == submission2[start2 + i]) {
            ++matches;
        }
    }
    return (static_cast<double>(matches) / length) >= 0.8;
    
}


std::array<int, 5> match_submissions(std::vector<int> &submission1, 
        std::vector<int> &submission2) {
    // TODO: Write your code here
    int n = submission1.size(), m = submission2.size();
    std::array<int, 5> result = {0, 0, 0, 0, 0};
    std::cout << "m = " << m << " n = " << n << "\n" ;
    for(int length=0; length<std::min(m,n); length++){
        for(int i=0; i<n-length+1; i++){
            for(int j=0; j<m-length+1;j++){
                bool is_match = false;
                {
                    is_match = checkExactMatch(submission1,submission2,i,j,length);
                    if(is_match){
                        result[1] = length;
                        result[3] = i;
                    }
                }
                {
                    is_match = checkApproximateMatch(submission1,submission2,i,j,length);
                    if (is_match) {
                        result[2] = length;
                        result[4] = i;
                    }
                }
            }
        }
    }
    for(int i = 0; i < 5; i++) {
        std::cout << result[i] << ' ';
    }
    std::cout << "\n";
    return result; // dummy return
    // End TODO
}
