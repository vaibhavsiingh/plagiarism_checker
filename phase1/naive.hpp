#include <array>
#include <iostream>
#include <vector>
#include<cmath>
#include<unordered_set>

// Function to find the longest fuzzy matching subsequence
std::array<int, 5> match_submissions(std::vector<int> &arr1, std::vector<int> &arr2) {
    if (arr1.empty() || arr2.empty()) {
        return {0, 0, 0, 0, 0};
    }
    
    std::array<int, 5> result = {0, 0, 0, 0, 0};  // Result array
    
    int m = arr1.size();
    int n = arr2.size();
    
    std::unordered_set<int>s;

    for(int size=20; size>=10; size--){
        for(int i=0; i<m-size+1; i++){
            for(int j=0; j<n-size+1; j++){
                bool is_match = true;
                for(int k=0; k<size; k++){
                    if(arr1[i+k] != arr2[j+k]){
                        is_match = false;
                        break;
                    }
                    if(s.count(j)){
                        is_match = false;
                        break;
                    }
                }
                if(is_match){
                    result[1] += size;
                    for(int k=0; k<size; k++){
                        s.insert(j+k);
                    }
                }
            }
        }
    }
   
    return result;
}
