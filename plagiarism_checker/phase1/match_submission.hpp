#include <array>
#include <iostream>
#include <span>
#include <vector>
#include <cmath>
// -----------------------------------------------------------------------------

#include <unordered_set>
#include <algorithm>

// Constants defining match criteria
#define MIN_PERFECT_MATCH 10
#define MIN_APPROX_MATCH 30
#define MATCH_THRESHOLD 0.8


// RollingHash class to compute rolling hash values for a sequence
class RollingHash {
private:
    std::vector<long long> hash, power;
    static constexpr int BASE = 257;
    static constexpr int MOD = 1e9 + 7;
public:
    // Initializes rolling hash values for the input sequence
    RollingHash(const std::vector<int>& str) {
        int n = str.size();
        hash.resize(n + 1);
        power.resize(n + 1);

        hash[0] = 0;
        power[0] = 1;

        for (int i = 0; i < n; i++) {
            hash[i + 1] = ((hash[i] * BASE) + str[i]) % MOD;
            power[i + 1] = (power[i] * BASE) % MOD;
        }
    }

    // Computes hash for a specified subsequence
    long long getHash(const int start_index, const int length) const {
        long long h = (hash[start_index + length] - (hash[start_index] * power[length] % MOD) + MOD) % MOD;
        return h;
    }

    // Generates hashes for all substrings of a given length
    std::vector<long long> generateHashes(const int length = MIN_PERFECT_MATCH) const {
        std::vector<long long> hashes;
        if (hash.size() < length) return hashes;
        for (int i = 0; i <= hash.size() - length - 1; i++) {
            hashes.push_back(getHash(i, length));
        }
        return hashes;
    }
};

// Checks the length of a perfect match starting from two indices in separate sequences
int checkMatchLength(const std::vector<int>& vec1, int start1,
                     const std::vector<int>& vec2, int start2) {
    int matchLength = 0;
    while (start1 + matchLength < vec1.size() && start2 + matchLength < vec2.size() &&
           vec1[start1 + matchLength] == vec2[start2 + matchLength]) {
        matchLength++;
    }
    return matchLength;
}


// Finds exact matches between two sequences and returns the total matched length
int findExactMatches(const std::vector<int>& submission1, const std::vector<int>& submission2,
                     const int min_length = MIN_PERFECT_MATCH) {
    RollingHash rolling_hash1(submission1), rolling_hash2(submission2);
    std::vector<long long> hashes1 = rolling_hash1.generateHashes(min_length);
    std::vector<long long> hashes2 = rolling_hash2.generateHashes(min_length);

    int total_matched_length = 0;
    std::unordered_set<int> matched_indices1, matched_indices2;

    for (int i = 0; i < hashes1.size(); i++) {
        for (int j = 0; j < hashes2.size(); j++) {
            if (hashes1[i] == hashes2[j] && matched_indices1.count(i) == 0 &&
                matched_indices2.count(j) == 0) {
                int match_length = checkMatchLength(submission1, i, submission2, j);
                if (match_length >= min_length) {
                    for (int k = 0; k < match_length; k++) {
                        matched_indices1.insert(i + k);
                        matched_indices2.insert(j + k);
                    }
                    i += match_length - 1;
                    total_matched_length += match_length;
                    break;
                }
            }
        }
    }

    return total_matched_length;
}



// Counts mismatches within a specified length from given starting indices in two sequences
int countMismatches(const std::vector<int>& vec1, const std::vector<int>& vec2,
                    int start1, int start2, int length) {
    int mismatches = 0;
    for (int k = 0; k < length; k++) {
        if (vec1[start1 - k] != vec2[start2 - k]) {
            mismatches++;
        }
    }
    return mismatches;
}


// Finds the longest fuzzy match allowing limited mismatches within 80% and returns the starting indices as well
std::array<int, 3> findLongestFuzzyMatch(const std::vector<int>& vec1, 
                                         const std::vector<int>& vec2) {
    int n = vec1.size();
    int m = vec2.size();
    // Creating a 2D DP array
    std::vector<std::vector<int>> dp(n + 1, std::vector<int>(m + 1, 0));
    int max_length = 0;
    int start_index1 = -1;
    int start_index2 = -1;

    // Iterate through each position in both vectors
    for (int i = 1; i <= n; i++) {
        for (int j = 1; j <= m; j++) {
            if (vec1[i - 1] == vec2[j - 1]) {
                // If elements match, extend the match length from the previous position
                dp[i][j] = dp[i - 1][j - 1] + 1;
            } else {
                // If elements do not match, consider a "fuzzy" match with mismatches
                int length = dp[i - 1][j - 1] + 1;
                int max_mismatches = static_cast<int>(length * (1 - MATCH_THRESHOLD));
                int mismatches = countMismatches(vec1, vec2, i - 1, j - 1, length);

                if (mismatches <= max_mismatches) {
                    dp[i][j] = length;
                } else {
                    dp[i][j] = 0;
                }
            }

            // Update longest match details if the current match length is the largest found
            if (dp[i][j] > max_length) {
                max_length = dp[i][j];
                start_index1 = i - max_length;
                start_index2 = j - max_length;
            }
        }
    }

    if(max_length<MIN_APPROX_MATCH){
        max_length = 0;
        start_index1 = -1;
        start_index2 = -1;
    }
    return {max_length, start_index1, start_index2};
}

std::array<int, 5> match_submissions(std::vector<int>& submission1, std::vector<int>& submission2) {
    auto [fuzzy_match_length, start_index1, start_index2] = findLongestFuzzyMatch(submission1, submission2);
    size_t min_size = std::min(submission1.size(), submission2.size());
    int total_match_length = findExactMatches(submission1, submission2);

    bool is_plagiarized = (total_match_length >= (0.15 * min_size) && fuzzy_match_length >= (0.3 * min_size)) || 
                         total_match_length >= 300 || fuzzy_match_length >= 250;

    return {is_plagiarized ? 1 : 0, total_match_length, fuzzy_match_length, start_index1, start_index2};
}
#include <array>
#include <iostream>
#include <span>
#include <vector>
#include <cmath>
#include<unordered_map>
#include<algorithm>
#include<unordered_set>


// -----------------------------------------------------------------------------

// You are free to add any STL includes above this comment, below the --line--.
// DO NOT add "using namespace std;" or include any other files/libraries.
// Also DO NOT add the include "bits/stdc++.h"

// OPTIONAL: Add your helper functions and data structures here
//this is used to generate polynomial hashes.
std::size_t polynomial_hash(const std::vector<int>& vec, int base = 31, int mod = 1e9 + 9) {
    std::size_t hash_value = 0;
    std::size_t power = 1;
    for (const auto& num : vec) {
        hash_value = (hash_value + num * power) % mod;
        power = (power * base) % mod;
    }
    return hash_value;
}
//winnowing function is used to check if there is a significant match this function returns number of matched elements in first vector to second vector.
//for smaller lengths i am taking k gram length 5 and doing directly inside the function total winnowing process.
int winnowing_for_smaller_elements(const std::vector<int>& vector1, const std::vector<int>& vector2){
    int window_size=5;
    std::vector<int>winnowingmap1;
    std::vector<int>winnowingmap2;
    for(int i=0;i+5<vector1.size();i++)
{
std::vector<int>subseq(vector1.begin()+i,vector1.begin()+i+5);
winnowingmap1.push_back(polynomial_hash(subseq));
}
for(int i=0;i+5<vector2.size();i++){
    std::vector<int>subseq(vector2.begin()+i,vector2.begin()+i+5);
    winnowingmap2.push_back(polynomial_hash(subseq));}
std::vector<int> minwinnowingmap1;
std::vector<int> minwinnowingmap2;
for (std::size_t i = 0; i + 5 <= winnowingmap1.size(); ++i) {
    int min_value = *std::min_element(winnowingmap1.begin() + i, winnowingmap1.begin() + i + 5);
    minwinnowingmap1.push_back(min_value);       
    
}
for(int i=0;i+5<winnowingmap2.size();i++){
    int min_value = *std::min_element(winnowingmap2.begin() + i, winnowingmap2.begin() + i + 5);
    minwinnowingmap2.push_back(min_value);  
}
std::unordered_map<std::size_t, int> hash_map;
for(int i=0;i<minwinnowingmap1.size();i++){ 
        hash_map[minwinnowingmap1[i]]++;
    }
    for(int i=0;i<minwinnowingmap2.size();i++){
        if(hash_map.find(minwinnowingmap2[i])!=hash_map.end()&&hash_map[minwinnowingmap2[i]]>0){
           
            hash_map[minwinnowingmap2[i]]--;
        }
    }
    int counthere=0;
    for(auto it:hash_map){
        counthere+=it.second;
    }
    
    return minwinnowingmap1.size()-counthere; // No matching subsequences found

}
//for larger cases to decrease time i calculated k gram hashes and min sliding window hashes in the main function(int main) and passed to this function.
//and then i am passing minimum sliding window hashes to this fuction to significantly decrease time.
//this function returns number of matched elements in first vector to second vector.
int winnowing(const std::vector<int>& vector1, const std::vector<int>& vector2) {
    int window_size = 10;
    std::unordered_map<std::size_t, int> hash_map;
    
  
    //i am using a vector map to store the values of vector1 in hashmap.
    for(int i=0;i<vector1.size();i++){ 
        hash_map[vector1[i]]++;
    }
    //here i am checking if there is a match in the vector2 with the vector1 and if there is a match i will decrease the value in he hashmap by 1.
    for(int i=0;i<vector2.size();i++){
        if(hash_map.find(vector2[i])!=hash_map.end()&&hash_map[vector2[i]]>0){
           
            hash_map[vector2[i]]--;
        }
    }
    int counthere=0;
    //finally i am counting number of unmatched.
    for(auto it:hash_map){
        counthere+=it.second;
    }
    
    return vector1.size()-counthere; // No matching subsequences found
}

std::array<int, 5> match_submissions(std::vector<int> &submission1, 
        std::vector<int> &submission2) {
    // TODO: Write your code here
    
    std::array<int, 5> result = {0, 0, 0, 0, 0};
    std::vector<bool> visitedin1(submission1.size(), false);
    std::vector<bool> visitedin2(submission2.size(), false);
    std::unordered_map<std::size_t, std::vector<int>> hash_map;
    // Compute hashes for all subsequences of length 10 in submission1(contigious subseq)
    for (std::size_t i = 0; i + 10 <= submission1.size(); ++i) {
        std::vector<int> subsequence(submission1.begin() + i, submission1.begin() + i + 10);
        std::size_t hash = polynomial_hash(subsequence);
        hash_map[hash].push_back(i);
    }
    int subseq_length = 10;
    // here i am computing hashes for subsequences of length 10 in submission2(contigious subseq)
    //and checking if there is a same hash in the submission1 hashes
    //if there is a subsequence match i am filling visitedin1 vector with true for matched 10 indices in both vectors visitedin1 and visitedin2.
    //i am using visitedin1 and visitedin2 because we should not do overcounting as mentioned in question.
    //here all the multiples of 10 subsequences will be captured and subsequences of lenth 13/14/15...will not be matched.for that i am doind in next part.
    for (int i = 0; i+subseq_length <= submission2.size(); ++i) {
       if(!visitedin2[i]){ std::vector<int> subseq(submission2.begin() + i, submission2.begin() + i + subseq_length);
        std::size_t hash_value = polynomial_hash(subseq);
        
        if (hash_map.find(hash_value) != hash_map.end()) {
            for (int start_index : hash_map[hash_value]) {
                if (!visitedin1[start_index] &&!visitedin1[start_index+subseq_length-1]&& !visitedin2[i]&&!visitedin2[i+subseq_length-1]) {
                    for(int pp=0;pp<subseq_length;pp++){visitedin1[start_index+pp] = true;
                    visitedin2[i+pp] = true;}
                    // Increment the count of matched subsequences
                }
            }
        }
    }}
    
//take a 15 length subseq we want to match all the 15 but from before step we only match first 10 and the next 5 will be left .
//now the basic idea is to travese the submission1 and submission2 reversely and match.
//them in reverse travesal visitedin1 and visitedin2 will be false for the wanted match so we can match it here.
//so i reversed all submission1,submission2,visitedin1 and visitedin2.
std::vector<int> reversed_submission1(submission1.rbegin(), submission1.rend());
std::vector<int> reversed_submission2(submission2.rbegin(), submission2.rend());
std::unordered_map<std::size_t, std::vector<int>> newhashmap;
std::vector<bool> revvisitedin1(visitedin1.rbegin(), visitedin1.rend());
std::vector<bool> revvisitedin2(visitedin2.rbegin(), visitedin2.rend());

//i am computing hashes for the reverse subsequences.
for (std::size_t i = 0; i + 10 <= reversed_submission1.size(); ++i) {
    std::vector<int> subsequence(reversed_submission1.begin() + i, reversed_submission1.begin() + i + 10);
    std::size_t hash = polynomial_hash(subsequence);
    newhashmap[hash].push_back(i);
}

//now i am checking if a match is there.
//here there is a different case take a subseq of length 10 A in subm1 
//and there is other subseq of length 10 in subm1.here A and B are in such
//a way that last 4 numbers of a are first 4 numbers of B and the next 6 numbers after A are last 6 numbers of B.
//If we naively traverse like before we count this case which should not be counted.
//so before making visited true. I am checking if this case is excluded by traversing forward and checking all the between hashes are matched.
for (int i = 0; i+subseq_length<= submission2.size() ; ++i) {
    if(!revvisitedin2[i]){
    std::vector<int> subseq(reversed_submission2.begin() + i, reversed_submission2.begin() + i + subseq_length);
    std::size_t hash_value = polynomial_hash(subseq);
    if(newhashmap.find(hash_value)!=newhashmap.end()){
        for(int start_index:newhashmap[hash_value]){
            if(!revvisitedin1[start_index]&&!revvisitedin2[i]){
                int startindexin1=start_index;
                int startindexin2=i;
                while(startindexin1 < reversed_submission1.size() && startindexin2 < reversed_submission2.size() && startindexin1 + subseq_length - 1 < reversed_submission1.size() && startindexin2 + subseq_length - 1 < reversed_submission2.size()){
                    std::vector<int> subseq1(reversed_submission2.begin() + startindexin2, reversed_submission2.begin() + startindexin2 + subseq_length);
                    std::size_t hash_valuenow = polynomial_hash(subseq1);
                     if(revvisitedin1[startindexin1]&&revvisitedin2[startindexin2]){
                        
                        for(int j=start_index;j<=startindexin1;j++){revvisitedin1[j]=true;}
                        for(int j=i;j<=startindexin2;j++){revvisitedin2[j]=true;}
                        //here found match should modify visited
                        ;break;
                     }
                     if(revvisitedin1[startindexin1]&&!revvisitedin2[startindexin2]){break;}
                     if(!revvisitedin1[startindexin1]&&revvisitedin2[startindexin2]){break;}
                     if(newhashmap.find(hash_valuenow)!=newhashmap.end()){
                            startindexin1+=1;
                            startindexin2+=1;
                     }

                }
            }
        }
    }
    }
}

int count4=0;


//finally i am counting total number of exact matches by counting total visited indexes in revvisitedin1.
for (bool visited : revvisitedin1) {
    if (visited) {
        count4++;
    }
}

//for approximate matching i am using "winnowing" which is one of the idea in actual moss.
//i read about winnowing from this the algorithm and why the algorithm produces good results is expalained in 
//this under section winnowing and expected density "https://theory.stanford.edu/~aiken/publications/papers/sigmod03.pdf"
//The process starts by breaking the sequence into small overlapping chunks called shingles. 
//Each shingle is then converted into a numeric representation using hashing.
// Next, a sliding window moves across the sequence of hashes,
// and only the smallest hash (known as the min-hash) from each window is selected as part of the fingerprint.
// This selection process reduces the total data and helps focus on the sequence's most representative features.
//
//first i am computing all the hashes of all the k grams in submission1 and submission2 and store them in winnowinmap1 and winnowingmap2.
//next i am using a sliding window of size 10 and storing the minimum hash value in the window in minwinnowingmap1 and minwinnowingmap2.


// Using winnowing to find the longest matching subsequence between two sequences

// Step 1: First, we generate hashes for all subsequences of length 10 using a polynomial hash function.
//         This gives each 10-character chunk of the sequence a unique identifier.

// Step 2: Next, we slide a window of size 10 over each sequence to find the smallest hash value in each window.
//         We store these minimum values in vectors `minWinnowingMap1` and `minWinnowingMap2` for each sequence.
//         By keeping only the smallest values, we make it faster and easier to compare sequences later on.

// Step 3: Now, we go through each subsequence length (i) in `minWinnowingMap1` and `minWinnowingMap2`
//         to look for matches. This helps us spot similar parts between the two sequences.

// Step 4: If we find enough matches between `minWinnowingMap1` and `minWinnowingMap2`, we consider the sequences
//         to have an approximate match. 

// Step 5:i am using the "winnowing" fuction to return number of unmatched elements in the first vector passed to it.

// Step 6: Finally, we use binary search to zero in on the longest matching subsequence length.
//         By adjusting the subsequence length we're comparing, binary search helps us quickly find the longest match.

// Overall, this method identifies approximate matches by comparing only key hash values, 
// making it a lot faster to find the longest shared subsequence between sequences.




//winnowingmap1 one for calculating hashes of all k grams from submission1 and storing similarly winnowingmap2 for submission2. 
//minwinnowingmap1 and minwinnowingmap2 are used to store the minimum hash values of the k grams in the window of 10.
std::vector<int>winnowingmap1;
std::vector<int>winnowingmap2;
for(int i=0;i+10<submission1.size();i++)
{
std::vector<int>subseq(submission1.begin()+i,submission1.begin()+i+10);
winnowingmap1.push_back(polynomial_hash(subseq));
}
for(int i=0;i+10<submission2.size();i++){
    std::vector<int>subseq(submission2.begin()+i,submission2.begin()+i+10);
    winnowingmap2.push_back(polynomial_hash(subseq));   
}
std::vector<int> minwinnowingmap1;
for (std::size_t i = 0; i + 10 <= winnowingmap1.size(); ++i) {
    int min_value = *std::min_element(winnowingmap1.begin() + i, winnowingmap1.begin() + i + 10);
    minwinnowingmap1.push_back(min_value);
}
std::vector<int> minwinnowingmap2;
for (std::size_t i = 0; i + 10 <= winnowingmap2.size(); ++i) {
    int min_value = *std::min_element(winnowingmap2.begin() + i, winnowingmap2.begin() + i + 10);
    minwinnowingmap2.push_back(min_value);
}

int final_longest_approxmatch_length=0;int final_start_index_in_submission1=0;int final_start_index_in_submission2=0;


int first=30;
int last=minwinnowingmap1.size();
int mid=(first+last)/2;

while(first<last){
    
    bool found=false;
    int i=mid;
    
       if(i>1000){
        for(int p=0;p+i<=minwinnowingmap1.size();p=p+50){
        std::vector<int>subseq1(minwinnowingmap1.begin()+p,minwinnowingmap1.begin()+p+i);
        for(int q=0;q+i<=minwinnowingmap2.size();q=q+50){
            
           std::vector<int>subseq2(minwinnowingmap2.begin()+q,minwinnowingmap2.begin()+q+i);
            int numofmatches=winnowing(subseq1,subseq2);
            double similarity=numofmatches*100.0/(subseq1.size()+subseq2.size()-numofmatches);
            if(similarity>80){
                
                found=true;
            final_longest_approxmatch_length=i;final_start_index_in_submission2=p;final_start_index_in_submission1=q;
              break;
            }
        }if(found){break;}
    }
       }
       else if(i>150){for(int p=0;p+i<=minwinnowingmap1.size();p=p+10){
        std::vector<int>subseq1(minwinnowingmap1.begin()+p,minwinnowingmap1.begin()+p+i);
        for(int q=0;q+i<=minwinnowingmap2.size();q=q+10){
            
           std::vector<int>subseq2(minwinnowingmap2.begin()+q,minwinnowingmap2.begin()+q+i);
            int numofmatches=winnowing(subseq1,subseq2);
            double similarity=numofmatches*100.0/(subseq1.size()+subseq2.size()-numofmatches);
            if(similarity>85){
                
                found=true;
            final_longest_approxmatch_length=i;final_start_index_in_submission2=p;final_start_index_in_submission1=q;
              break;
            }
        }if(found){break;}
    }}
    else{
        for(int p=0;p+i<=submission1.size();p=p+10){
        std::vector<int>subseq1(submission1.begin()+p,submission1.begin()+p+i);
        for(int q=0;q+i<=submission2.size();q=q+10){
            
           std::vector<int>subseq2(submission2.begin()+q,submission2.begin()+q+i);
            int numofmatches=winnowing(subseq1,subseq2);
            double similarity=numofmatches*100.0/(subseq1.size()+subseq2.size()-numofmatches);
            if(similarity>92){
                
                found=true;
            final_longest_approxmatch_length=i;final_start_index_in_submission2=p;final_start_index_in_submission1=q;
              break;
            }
        }if(found){break;}
    }

    }

    if(found){
        first=mid+1;
    }
    else {
        last=mid;
    }
    mid=(first+last)/2;
}

//final plag check
result[0]=0;
//i am dividing int five cases that are below 200 tokens below 500 tokens,below 1000 tokens below 2000 tokens above 2000 tokens for treshholds for final plag check.
//submissions below 200 tokens 60% winnowing or 85 %exact match
//submissions below 500 tokens 40% winnowing or 75% exact match
//submissions below 1000 tokens 35% winnowing or 65% exact match
//submissions below 2000 tokens 30% winnowing or 50% exact match
//submissions above 2000 tokens 25% winnowing or 42% exact match 
if(submission1.size()==0||submission2.size()==0){
    result[0]=0;
}
else if(submission1.size()<200||submission2.size()<200){
    
int finalmatches=winnowing_for_smaller_elements(submission1,submission2);
double similarity=finalmatches*100.0/(submission1.size()+submission2.size()-finalmatches);
int minimumofsubmissions=std::min(submission1.size(),submission2.size());
if(similarity>60||count4>0.85*minimumofsubmissions){
    result[0]=1;

}

}
else if(submission1.size()<500||submission2.size()<500){
    int finalmatches=winnowing(minwinnowingmap1,minwinnowingmap2);
    double similarity=finalmatches*100.0/(minwinnowingmap1.size()+minwinnowingmap2.size()-finalmatches);
    int minimumofsubmissions=std::min(submission1.size(),submission2.size());
    if(similarity>40||count4>0.75*minimumofsubmissions){
        result[0]=1;
    }

}
else if(submission1.size()<1000||submission2.size()<1000){
int finalmatches=winnowing(minwinnowingmap1,minwinnowingmap2);
    double similarity=finalmatches*100.0/(minwinnowingmap1.size()+minwinnowingmap2.size()-finalmatches);
    int minimumofsubmissions=std::min(submission1.size(),submission2.size());
    if(similarity>30||count4>0.65*minimumofsubmissions){
        result[0]=1;
    }

}
else if(submission1.size()<2000||submission2.size()<2000){
    int finalmatches=winnowing(minwinnowingmap1,minwinnowingmap2);
    double similarity=finalmatches*100.0/(minwinnowingmap1.size()+minwinnowingmap2.size()-finalmatches);
    int minimumofsubmissions=std::min(submission1.size(),submission2.size());
    if(similarity>27||count4>0.5*minimumofsubmissions){
        result[0]=1;
    }

}
else{
int finalmatches=winnowing(minwinnowingmap1,minwinnowingmap2);
    double similarity=finalmatches*100.0/(minwinnowingmap1.size()+minwinnowingmap2.size()-finalmatches);
    int minimumofsubmissions=std::min(submission1.size(),submission2.size());
    if(similarity>24||count4>0.42*minimumofsubmissions){
        result[0]=1;
    }
 
}














result[1]=count4;
result[2]=final_longest_approxmatch_length;
result[3]=final_start_index_in_submission2;
result[4]=final_start_index_in_submission1;
    

    
    return result; // dummy return
    // End TODO
}
#include <array>
#include <iostream>
#include <span>
#include <vector>
#include <cmath>
// -----------------------------------------------------------------------------
#include <set>
#include <unordered_map>
#include <string>
#include<algorithm>
// You are free to add any STL includes above this comment, below the --line--.
// DO NOT add "using namespace std;" or include any other files/libraries.
// Also DO NOT add the include "bits/stdc++.h"

// OPTIONAL: Add your helper functions and data structures here



// OPTIONAL: Add your helper functions and data structures here
 // FOR THE FOLLOWING FUNCTION IMPLEMENTATION we adapted a part of code from geeksandgeeks. DP programming and LCS : https://www.geeksforgeeks.org/longest-common-subsequence-dp-4/

struct Result {
    int start1, end1, start2, end2, length;
    bool valid;
};

class SequenceMatcher {
public:
    // Main function to find a valid pair of sequences with LCS length meeting the threshold criteria
    static Result findValidSequences(const std::vector<int>& v1, const std::vector<int>& v2, double threshold = 0.8) {
        // Initialize an invalid Result by default
        Result result = {-1, -1, -1, -1, 0, false};

        // If either vector is empty, return the invalid result immediately
        if (v1.empty() || v2.empty()) return result;

        // Obtain the longest common subsequence as pairs of matching indices
        std::vector<std::pair<int, int>> lcs = getLCS(v1, v2);
        if (lcs.empty()) return result;  // Return if no common subsequence exists

        int maxLength = 0;  // Variable to track the maximum sequence length found

        // Explore pairs of indices in the LCS list to find the longest valid sequence within the threshold
        for (int i = 0; i < lcs.size(); ++i) {
            for (int j = i; j < lcs.size(); ++j) {
                int len1 = lcs[j].first - lcs[i].first + 1;  // Length of subsequence in v1
                int len2 = lcs[j].second - lcs[i].second + 1; // Length of subsequence in v2
                int matches = j - i + 1;  // Number of matching elements

                // Check if the matches meet the threshold for both v1 and v2 subsequence lengths
                if (matches >= threshold * len1 && matches >= threshold * len2) {
                    int currentLength = std::max(len1, len2);  // Determine the longer sequence length
                    // Update result if this sequence is the longest one found that meets the criteria
                    if (currentLength > maxLength) {
                        maxLength = currentLength;
                        result = {lcs[i].first, lcs[j].first, lcs[i].second, lcs[j].second, currentLength, true};
                    }
                }
            }
        }

        return result;  // Return the best found sequence or an invalid result if none met the criteria
    }

private:
    // Function to compute the LCS as pairs of indices (index from v1 and v2 for each matching element)
    static std::vector<std::pair<int, int>> getLCS(const std::vector<int>& a, const std::vector<int>& b) {
        int m = a.size(), n = b.size();
        std::vector<std::vector<int>> dp(m + 1, std::vector<int>(n + 1, 0));  // DP table for LCS length calculation

        // Fill the DP table based on matching elements between a and b
        for (int i = 1; i <= m; ++i) {
            for (int j = 1; j <= n; ++j) {
                if (a[i-1] == b[j-1]) {
                    dp[i][j] = dp[i-1][j-1] + 1;  // Extend LCS if elements match
                } else {
                    dp[i][j] = std::max(dp[i-1][j], dp[i][j-1]);  // Otherwise, take the longer of previous results
                }
            }
        }

        // Backtrack through the DP table to reconstruct the LCS as index pairs
        std::vector<std::pair<int, int>> lcs;
        int i = m, j = n;
        while (i > 0 && j > 0) {
            if (a[i-1] == b[j-1]) {
                lcs.push_back({i-1, j-1});  // Add matching indices to the LCS list
                --i; --j;  // Move diagonally in the DP table
            } else if (dp[i-1][j] > dp[i][j-1]) {
                --i;  // Move up in DP table if upper cell has greater LCS value
            } else {
                --j;  // Move left in DP table if left cell has greater LCS value
            }
        }
        
        std::reverse(lcs.begin(), lcs.end());  // Reverse to get indices in the correct order
        return lcs;
    }
};




//Function to find total length of matching substrings. FOR result[1]
std::unordered_map<std::string, std::vector<int>> get_n_length_substrings(const std::vector<int>& tokens, int n, std::set<int>& used_indices) {
    std::unordered_map<std::string, std::vector<int>> substrings;
    for (size_t i = 0; i + n <= tokens.size(); ++i) {
        // Check if this substring overlaps with already used indices
        bool overlap = false;
        for (int j = 0; j < n; ++j) {
            if (used_indices.count(i + j)) {
                overlap = true;
                break;
            }
        }
        if (overlap) continue;

        // Build a string representation of the n-length substring
        std::string pattern;
        for (int j = 0; j < n; ++j) {
            pattern += std::to_string(tokens[i + j]) + ",";
        }

        // Store the starting index for each unique pattern
        substrings[pattern].push_back(i);
    }
    return substrings;
}

// Function to find the total length of non-overlapping exact pattern matches between lengths 10 and max_match_len
int find_total_match_length(const std::vector<int>& submission1, const std::vector<int>& submission2, int min_match_len= 10, int max_match_len = 29) {
    int total_length = 0;
    std::set<int> used_indices1, used_indices2; // Track used indices in each vector

    // Loop through each length from max_match_len to min_match_len
    for (int n = max_match_len; n >= min_match_len; --n) {
        // Get all non-overlapping n-length substrings for both submissions
        std::unordered_map<std::string, std::vector<int>> patterns1 = get_n_length_substrings(submission1, n, used_indices1);
        std::unordered_map<std::string, std::vector<int>> patterns2 = get_n_length_substrings(submission2, n, used_indices2);

        // Find common patterns and accumulate their lengths
        for (const auto& entry : patterns1) {
            const std::string& pattern = entry.first;

            // Check if the pattern exists in both submissions
            if (patterns2.find(pattern) != patterns2.end()) {
                // Process each occurrence of the pattern in submission1
                for (int idx1 : patterns1[pattern]) {
                    // If indices overlap in submission1, skip this match
                    bool overlap1 = false;
                    for (int j = 0; j < n; ++j) {
                        if (used_indices1.count(idx1 + j)) {
                            overlap1 = true;
                            break;
                        }
                    }
                    if (overlap1) continue;

                    // Process each occurrence of the pattern in submission2
                    for (int idx2 : patterns2[pattern]) {
                        bool overlap2 = false;
                        for (int j = 0; j < n; ++j) {
                            if (used_indices2.count(idx2 + j)) {
                                overlap2 = true;
                                break;
                            }
                        }
                        if (overlap2) continue;

                        // If no overlap, count this match and mark indices as used
                        total_length += n;
                        for (int j = 0; j < n; ++j) {
                            used_indices1.insert(idx1 + j);
                            used_indices2.insert(idx2 + j);
                        }

                        // Break after finding one match to avoid multiple counting for the same pattern
                        break;
                    }
                }
            }
        }
    }

    return total_length;
}


std::array<int, 5> match_submissions(std::vector<int> &submission1, 
        std::vector<int> &submission2) {
    // TODO: Write your code here
    std::array<int, 5> result = {-1, 0, -1, -1, -1};
    result[1] = find_total_match_length(submission1,submission2);
    Result longest = SequenceMatcher::findValidSequences(submission1, submission2);  //class RESULT IS DEFINED EARLIER 
    // result[2] = longest.maxLength;
    // result[3] = longest.index1;
    // result[4] = longest.index2;
    if(longest.length > 0) {
        result[2] = longest.length;
        result[3] = longest.start1;
        result[4] = longest.start2;
    }

    int min  = (submission1.size()>submission2.size())? submission2.size() : submission1.size();


    if( result[1] / static_cast<double>(min) >= 0.2   || result[2]/static_cast<double>(min) >= 0.3){
        result[0] = 1;

    }
    else{
        result[0] = 0;
    }
    return result; 
    // End TODO  
}
#include <array>
#include <iostream>
#include <span>
#include <vector>
#include <cmath>
// -----------------------------------------------------------------------------
// #include <stdexcept>
#include <algorithm> 
#include <map>
#define k 10 // for exact match

// You are free to add any STL includes above this comment, below the --line--.
// DO NOT add "using namespace std;" or include any other files/libraries.
// Also DO NOT add the include "bits/stdc++.h"

// OPTIONAL: Add your helper functions and data structures here
class Exact_Match{
    public:
        // returns hash values of the text
        // Hash computation is inspired from GeeksForGeeks - "https://www.geeksforgeeks.org/rabin-karp-algorithm-for-pattern-searching/"
        static std::map<int, std::vector<int>> hash_text(std::vector<int> text)
        {
            std::map<int, std::vector<int>> hash_map;

            int n = text.size();
            int p = 5381, d = 33;
            int h = 1;

            for (int i = 0; i < 10 - 1; i++) h = (h * d) % p;

            int t = 0;
            for (int i = 0; i < 10; i++) t = (d * t + text[i]) % p;

            if (hash_map.find(t) == hash_map.end()) hash_map[t] = {0};
            else hash_map[t].push_back(0);

            // Uses rolling hash function for hash computation efficiency
            for (int i = 1; i < n - 10; i++){
                t = (d * (t - text[i - 1] * h) + text[i + 9]) % p;
                if (t < 0) t = t + p;
                if (hash_map.find(t) == hash_map.end()) hash_map[t] = {i};
                else hash_map[t].push_back(i);
            }

            return hash_map;
        }

        // returns hash values of the pattern
        // Hash computation is inspired from GeeksForGeeks - "https://www.geeksforgeeks.org/rabin-karp-algorithm-for-pattern-searching/"
        static std::vector<int> hash_pattern(std::vector<int> &pattern)
        {
            int m = pattern.size();
            int h = 1;
            int p = 5381, d = 33;
            std::vector<int> hashes(m - k + 1);

            for (int i = 0; i < 10 - 1; i++) h = (h * d) % p;

            int t = 0;
            for (int i = 0; i < 10; i++) t = (d * t + pattern[i]) % p;

            hashes[0] = t;
            // Uses rolling hash function for hash computation efficiency
            for (int i = 1; i < m - 10; i++){
                t = (d * (t - pattern[i - 1] * h) + pattern[i + 9]) % p;
                if (t < 0)
                    t = t + p;
                hashes[i] = t;
            }

            return hashes;
        }

        // returns the length of the exact match
        static int exact_match(std::vector<int> &text, std::vector<int> &pattern)
        {
            // calls the hash functions
            std::map<int, std::vector<int>> text_hash = hash_text(text);
            std::vector<int> pattern_hash = hash_pattern(pattern);
            int p = pattern.size();
            int t = text.size();

            std::vector<bool> visited(t, false);

            int hashp;
            int total_length = 0;

            // iterates over the pattern index by index if there is no match
            for (int i = 0; i < p - k; i++)
            {   
                // matches the hash value of pattern and text
                hashp = pattern_hash[i];
                if (text_hash.find(hashp) != text_hash.end())
                {
                    std::vector<int> indices = text_hash[hashp];
                    int max = k;
                    int index = -1;

                    // runs for maximum match length over each match of hash value
                    for (auto j : indices)
                    {
                        int len = k;
                        bool ismatch = true;

                        // checks for feasible matches (considering overlap conditions)
                        for (int z = 0; z < 10; z++)
                        {
                            if (text[j + z] != pattern[i + z]) ismatch = false;
                            if (visited[j + z] == true) ismatch = false;
                            if(ismatch == false) break;
                        }
                        
                        // if match is feasible, checks for further indices 
                        if(ismatch){
                            while ((j + len < t) && (text[j + len] == pattern[i + len]) && !(visited[j + len]))
                            {
                                len++;
                                if (len == 20)
                                    break;
                            }

                            if ((max <= len))
                            {
                                max = len;
                                index = j;
                            }
                        }   
                    }
                    // Updates if the feasible match has maximum length and adds to the total_length
                    if (index != -1)
                    {
                        for (int y = 0; y < max; y++) visited[index + y] = true;
                        total_length = total_length + max;
                        i = i + max - 1;
                    }
                }
            }
            return total_length;
        }
};

class Approx_Match
{
public:
    // returns the starts indices of both the submissions where the largest approx match is found along with its length
    // returns as a vector (start1, start2, max_approx_length)
    static std::vector<int> longest_80(std::vector<int> ind1, std::vector<int> ind2)
    {
        int s = 0;
        int e = ind1.size()-1;
        int max = 0;
        int sub1i = ind1[s];
        int sub2i = ind2[s];
        // Iterates over all possible aprrox length sub vectors and finds the vector with maximum length
        for(int i=s;i<ind1.size();i++){
            for(int j=e;j>i;j--){
                if((j-i+1)>0.8*(ind1[j]-ind1[i]+1)) {
                    if(max < ind1[j]-ind1[i]+1){
                        sub1i = ind1[i];
                        sub2i = ind2[i];
                        max = ind1[j]-ind1[i]+1;
                    }
                }
            }
        }
        return {sub1i,sub2i,max};
    }

    // This algorithm is inspired from GeeksForGeeks website "https://www.geeksforgeeks.org/longest-common-subsequence-dp-4/"
    static std::vector<int> findLCS(const std::vector<int> &vec1, const std::vector<int> &vec2)
    {
        int m = vec1.size();
        int n = vec2.size();

        // dp[i][j] stores the length of LCS for vec1[0..i-1] and vec2[0..j-1]
        std::vector<std::vector<int>> dp(m + 1, std::vector<int>(n + 1, 0));

        // Fill dp table
        for (int i = 1; i <= m; i++)
        {
            for (int j = 1; j <= n; j++)
            {
                if (vec1[i - 1] == vec2[j - 1])
                {
                    dp[i][j] = dp[i - 1][j - 1] + 1;
                }
                else
                {
                    dp[i][j] = std::max(dp[i - 1][j], dp[i][j - 1]);
                }
            }
        }

        // Reconstruct the LCS from dp table
        std::vector<int> lcs;
        int i = m, j = n;
        std::vector<int> text_indices;
        std::vector<int> pat_indices;

        while (i > 0 && j > 0)
        {
            // If current characters match, add to result
            if (vec1[i - 1] == vec2[j - 1])
            {   
                text_indices.push_back(i-1);
                pat_indices.push_back(j-1);
                i--;
                j--;
            }
            // Move in direction of larger value
            else if (dp[i - 1][j] > dp[i][j - 1]) i--;
            else j--;
        }
        std::reverse(text_indices.begin(), text_indices.end());
        std::reverse(pat_indices.begin(), pat_indices.end());
        return longest_80(text_indices,pat_indices);
    }
};

//returns result[0] analysing result[1],result[2] and token vector sizes
int determine_plag_flag(int result1, int result2, int n1, int n2) {
    // Calculate percentage of matched content relative to the size of the smaller file
    double match_percentage = static_cast<double>(result1) / std::min(n1, n2) * 100;
    
    // Calculate the ratio of file sizes
    double size_ratio = static_cast<double>(std::max(n1, n2)) / std::min(n1, n2);
    
    // DEclaring three kinds of conditions for determining plagiarism flag
    bool plag_conditions[3];
    
    if (std::min(n1, n2) < 150) {
        // Handle smaller files differently (less than 150 tokens or so)
        plag_conditions[0] = (match_percentage > 70);                     // High match percentage
        plag_conditions[1] = false;          // Long continuous match (Not so relevant for smaller codes )
        plag_conditions[2] = false;                                       // Not needed for small files
    } else {
        // Conditions for larger files
        plag_conditions[0] = (match_percentage > 50 );    // High match percentage with enough length
        plag_conditions[1] = (result2 > std::min(n1, n2) * 0.8); // Very long continuous match
        plag_conditions[2] = (match_percentage > 45 && size_ratio < 1.3 && result1 > 150); // Moderate match with similar size
    }
    
    // Set plag flag based on each of the plag_conditions analysed above
    return (plag_conditions[0] || plag_conditions[1] || plag_conditions[2]) ? 1 : 0;
}

std::array<int, 5> match_submissions(std::vector<int> &submission1, std::vector<int> &submission2) {
    // TODO: Write your code here
    std::array<int, 5> result = {0, 0, 0, 0, 0};
    std::vector<int> approx_reply;
    int n1 = submission1.size();
    int n2 = submission2.size();

    if(n1>n2) {
        result[1]=Exact_Match::exact_match(submission1,submission2);
        approx_reply = Approx_Match::findLCS(submission1,submission2);
        result[2] = approx_reply[2];
        result[3] = approx_reply[0];
        result[4] = approx_reply[1];
        
    } 
    else {
        result[1]=Exact_Match::exact_match(submission2,submission1);
        approx_reply = Approx_Match::findLCS(submission2,submission1);
        result[2] = approx_reply[2];
        result[3] = approx_reply[1];
        result[4] = approx_reply[0];
        
    }
    result[0] = determine_plag_flag(result[1], result[2], n1, n2);
    return result; 
    // End TODO
}













#include <array>
#include <iostream>
#include <span>
#include <vector>
#include <cmath>
#include <map>
#include <memory>
#include <algorithm>
#include <tuple>
#include <cmath>
#include <unordered_map>
#include <cassert>
#include <chrono>

// -----------------------------------------------------------------------------

// You are free to add any STL includes above this comment, below the --line--.
// DO NOT add "using namespace std;" or include any other files/libraries.
// Also DO NOT add the include "bits/stdc++.h"

// OPTIONAL: Add your helper functions and data structures here

class TrieNode {
public:
    std::unordered_map<int, TrieNode*> children;
    int start;
    std::shared_ptr<int> end;
    int suffixIndex;
    TrieNode* suffixLink;
    bool isLeaf; 
    int children_count;

    TrieNode(int start, std::shared_ptr<int> end) 
        : start(start), end(end), suffixIndex(-1), suffixLink(nullptr), isLeaf(false) {}

    TrieNode(int start, std::shared_ptr<int> end, bool isLeaf) 
        : start(start), end(end), suffixIndex(-1), suffixLink(nullptr), isLeaf(isLeaf) {}

    int edgeLength() const {
        return *end - start + 1;
    }
};

class SimpleNode{
public:
    int value;
    std::unordered_map<int, SimpleNode*> children;

    SimpleNode(int v){
        value = v;
    }

    SimpleNode(){
        value = -1;
    }
};

// My implementation of the SuffixTree class is such that we should only ever insert codes of length 10 into this Trie,
// to keep track of which codes have already been seen once.
class NotAcceptingTrie{
public:
    SimpleNode* root;

    NotAcceptingTrie(){
        root = new SimpleNode();
    }

    void insert(const std::vector<int>& sequence, int count){
        SimpleNode* current = root;
        for (int i = 0; i < sequence.size(); i++){
            if (current->children.find(sequence[i]) == current->children.end()){
                current->children[sequence[i]] = new SimpleNode();
            }
            current = current->children[sequence[i]];
        }
        current->value = count;
    }

    bool check_if_present(const std::vector<int>& sequence){
        SimpleNode* current = root;
        for (int i = 0; i < sequence.size(); i++){
            if (current->children.find(sequence[i]) == current->children.end()){
                return false;
            }
            current = current->children[sequence[i]];
        }
        if (current->value == 0){
            return true;
        }
        current->value--;
        return false;
    }

    void freeTrieByPostOrder(SimpleNode* node){
        if (node == nullptr){
            return;
        }
        for (auto& child : node->children){
            freeTrieByPostOrder(child.second);
        }
        delete node;
    }

    ~NotAcceptingTrie(){
        freeTrieByPostOrder(root);
    }
};

class SuffixTree {
    // Implementation of this is based on Ukkonen's algorithm. I have referred to the implementation specified in https://www.geeksforgeeks.org/ukkonens-suffix-tree-construction-part-6/ and https://cp-algorithms.com/string/suffix-tree-ukkonen.html
    // Longest Common Substring is based on the algorithm explained in Wikipedia and on https://www.geeksforgeeks.org/suffix-tree-application-5-longest-common-substring-2/
    public:

    TrieNode* root;
    TrieNode* activeNode;
    TrieNode* lastNewNode;

    NotAcceptingTrie* not_accepted_trie;

    int activeEdge;
    int activeLength;

    int remainingSuffixCount;
    std::shared_ptr<int> leafEnd;
    std::unordered_map<int, int> children;

    std::shared_ptr<int> rootEnd;
    std::shared_ptr<int> splitEnd;
    int count;
    int size;

    std::vector<int> sequence;

    SuffixTree(const std::vector<int>& submission);

    ~SuffixTree();

    TrieNode* generateNewNode(int start, std::shared_ptr<int> end);
    
    bool walkDown(TrieNode* node);

    void extendSuffixTree(int position);

    void freeSuffixTreeByPostOrder(TrieNode* node);

    void setSuffixIndexByDFS(TrieNode* node, int label_height);

    void setNumberOfOccurencesByDFS(TrieNode* node);

    std::pair<int, int> deterministic_check(const std::vector<int>& submission2, std::vector<bool> plag_flags){
        // The algorithm works as follows: For every substring of length 10, if it matches with a substring in the base suffix tree, 
        // If it does match, we add the substring to a trie containing the sequences (and their respective counts) which should not be matched anymore
        // If the same sequence is seen again, decrement its count in the trie. For as long as the count is non-zero, we can "accept" the sequence
        // If the count becomes 0, reject the sequence whenever it occurs again
        // Here, on a rejection, the double-counting array comes into play, to ensure that we don't skip valid sequences but also that if an element is forced to map to a pattern which has already been used up,
        // we don't consider it as a new pattern but as a "double count"
        std::vector<bool> double_count_flags(plag_flags.size(), false);
        int start=0;
        int end=0;
        int gap=10;
        for (int i = 0; i + gap <= submission2.size(); i++){
            end=deterministic_check_from_starting_pos(submission2, i);
            if (end == 0) continue;
            // std::cout << end << "\n";
            if (!not_accepted_trie->check_if_present(std::vector<int>(submission2.begin()+i, submission2.begin()+i+gap))){
                // std::cout << "Accepted: ";
                // for (int j=i; j<i+gap; j++){
                //     std::cout << submission2[j] << " ";
                // }
                // std::cout << "\n";
                not_accepted_trie->insert(std::vector<int>(submission2.begin()+i, submission2.begin()+i+gap), end-1);
                for (int j=i; j<i+gap; j++){
                    plag_flags[j]=true;
                }
            }
            else {
                for (int j = i; j < i+gap; j++){
                    plag_flags[j]=true;
                    double_count_flags[j]=true;
                }
                // std::cout << "Rejected: ";
                // for (int j=i; j<i+gap; j++){
                //     std::cout << submission2[j] << " ";
                // }
                // std::cout << "\n";
            }
        }
        int plag_count=0;
        int longest=0;
        int max_longest=0;
        for (int i=0; i<plag_flags.size(); i++){
            // Toggle the commentedness of the line below for different logics
            // double_count_flags[i]=false;
            if (plag_flags[i] && !double_count_flags[i]){
                plag_count++;
            }
            if (plag_flags[i]){
                longest++;
            }
            else {
                max_longest=std::max(max_longest, longest);
                longest=0;
            }
        }
        max_longest=std::max(max_longest, longest);
        // std::cout << "Longest non-exact match found: " << max_longest << "\n";
        return {plag_count, max_longest};
    }

    int deterministic_check_from_starting_pos(const std::vector<int>& submission2, int starting_pos){
        TrieNode* current=root;
        if (current->children.find(submission2[starting_pos])==current->children.end()){
            return 0;
        }
        current=current->children[submission2[starting_pos]];
        int i=starting_pos;
        int start=current->start;
        int end=*current->end;
        while(true){
            while (i<submission2.size() && start<=end && submission2[i]==sequence[start]){
                i++;
                if (i==starting_pos+10){
                    return current->children_count;
                }
                start++;
            }
            if (i==submission2.size() || i==starting_pos+10){
                return current->children_count;
            }
            if (start <= end || current->children.find(submission2[i])==current->children.end()){
                return 0;
            }
            current=current->children[submission2[i]];
            start=current->start;
            end=*current->end;
        }
    }

    int traverse(TrieNode* node, int labelHeight, int* max_lenght){
        // Recursion to set flags (stored in the suffixIndex variable) to determine whether a particular node has children corresponding to both vector 1 and vector 2, only vector 1, only vector 2 or no children.
        if (node == nullptr){
            return 0;
        }
        int i = 0;
        int ret=-1;
        if (node->suffixIndex < 0){
            for (auto child : node->children){
                ret=traverse(child.second, labelHeight + child.second->edgeLength(), max_lenght);
                if (node->suffixIndex == -1){
                    node->suffixIndex = ret;
                }
                else if ((node->suffixIndex == -2 && ret == -3) || (node->suffixIndex == -3 && ret == -2) || (node->suffixIndex == -4)){
                    node->suffixIndex = -4;
                    if (labelHeight > *max_lenght){
                        *max_lenght = labelHeight;
                    }
                }
            }
        }
        else if (node->suffixIndex > -1 && node->suffixIndex < size){
            return -2;
        }
        else if (node->suffixIndex >= size){
            return -3;
        }
        return node->suffixIndex;
    }

    int longest_perfect_match(const std::vector<int>& submission2){
        // Creates the generalized suffix tree and calls traverse() on the same, assuming that -1 and -2 are distinguished values which can never be part of the tokens
        int original_size=sequence.size();
        sequence.push_back(-1);
        for (int i=0; i<submission2.size(); i++){
            sequence.push_back(submission2[i]);
        }
        sequence.push_back(-2);
        for (int i=original_size; i<sequence.size(); i++){
            extendSuffixTree(i);
        }
        setSuffixIndexByDFS(root, 0);

        int max_length=0;

        traverse(root, 0, &max_length);
        // std::cout << "Longest perfect match: " << max_length << "\n";
        return max_length;
    }
};

    TrieNode* SuffixTree::generateNewNode(int start, std::shared_ptr<int> end) {
        count++;
        TrieNode* node = new TrieNode(start, end);
        node->suffixLink = root;
        node->suffixIndex = -1;
        return node;
    }

    bool SuffixTree::walkDown(TrieNode* node) {
        if (activeLength >= node->edgeLength()) {
            activeEdge += node->edgeLength(); 
            activeLength -= node->edgeLength();
            activeNode = node;
            return true;
        }
        return false;
    }

    void SuffixTree::extendSuffixTree(int position) {
        // std::cout << std::make_shared<int>(leafEnd) << " " << &leafEnd << "\n";
        *leafEnd = position; 
        remainingSuffixCount++;
        lastNewNode = nullptr;
        
        while (remainingSuffixCount > 0) {
            if (activeLength == 0) {
                activeEdge = position;
            }

            int edgeKey = sequence[activeEdge];
            if (activeNode->children.find(edgeKey) == activeNode->children.end()) {
                activeNode->children[edgeKey] = generateNewNode(position, leafEnd); 
                if (lastNewNode != nullptr) {
                    lastNewNode->suffixLink = activeNode;
                    lastNewNode = nullptr;
                }
            } else {
                TrieNode* next = activeNode->children[edgeKey];
                if (walkDown(next)) {
                    continue;
                }

                if (sequence[next->start + activeLength] == sequence[position]) {
                    if (lastNewNode != nullptr && activeNode != root) {
                        lastNewNode->suffixLink = activeNode;
                        lastNewNode = nullptr;
                    }
                    activeLength++;
                    break;
                }

                auto splitEndPtr = std::make_shared<int>(next->start + activeLength - 1);
                TrieNode* split = generateNewNode(next->start, splitEndPtr);
                activeNode->children[edgeKey] = split;

                split->children[sequence[position]] = generateNewNode(position, leafEnd); 
                next->start += activeLength;
                split->children[sequence[next->start]] = next;

                if (lastNewNode != nullptr) {
                    lastNewNode->suffixLink = split;
                }
                lastNewNode = split;
            }

            remainingSuffixCount--;
            if (activeNode == root && activeLength > 0) {
                activeLength--;
                activeEdge = position - remainingSuffixCount + 1;
            } else if (activeNode != root) {
                activeNode = activeNode->suffixLink;
            }
        }
    }

    void SuffixTree::freeSuffixTreeByPostOrder(TrieNode* node) {
        if (node == nullptr) {
            return;
        }
        for (auto& child : node->children) {
            freeSuffixTreeByPostOrder(child.second);
        }
        delete node;
    }

    SuffixTree::~SuffixTree() {
        freeSuffixTreeByPostOrder(root);
        delete not_accepted_trie;
    }

    SuffixTree::SuffixTree(const std::vector<int>& submission) {
        for (int a : submission) {
            sequence.push_back(a);
        }
        activeEdge = -1;
        activeLength = 0;
        remainingSuffixCount = 0;
        leafEnd = std::make_shared<int>(-1);
        lastNewNode = nullptr;
        count = 0;

        rootEnd = std::make_shared<int>(-1);
        root = generateNewNode(-1, rootEnd); 
        activeNode = root;
        size=sequence.size();
        
        for (int i = 0; i < sequence.size(); i++) {
            extendSuffixTree(i);
        }
        setSuffixIndexByDFS(root, 0);
        setNumberOfOccurencesByDFS(root);
        not_accepted_trie = new NotAcceptingTrie();
    }

    void SuffixTree::setSuffixIndexByDFS(TrieNode* node, int label_height) {
        if (node == nullptr) {
            return;
        }
        int leaf = 1;
        for (auto child : node->children) {
            leaf = 0;
            setSuffixIndexByDFS(child.second, label_height + child.second->edgeLength());
        }
        if (leaf == 1) {
            node->suffixIndex = sequence.size() - label_height;
            // std::cout << node->suffixIndex << "\n";
        }
        else {
            node->suffixIndex = -1;
        }
    }

    void SuffixTree::setNumberOfOccurencesByDFS(TrieNode* node) {
        if (node == nullptr) {
            return;
        }
        int leaf = 1;
        for (auto child : node->children) {
            leaf = 0;
            setNumberOfOccurencesByDFS(child.second);
        }
        if (leaf == 1) {
            node->children_count = 1;
        } else {
            node->children_count = 0;
            for (auto child : node->children) {
                node->children_count += child.second->children_count;
            }
        }
    }

std::vector<std::tuple<int, double, int, int>> smith_waterman_80_similarity(const std::vector<int>& s1, const std::vector<int>& s2, int match = 3, int gap = -2, const std::vector<double>& thresholds={0.8}) {
    // This algorithm, commonly used in bioinformatics, is used to find a location with high local alignment.
    // I am using extra 2D vectors in order to carry out the check for a particular location having a greater than 80% match before potentially changing the max_index.
    assert(std::abs(thresholds[0]-0.8)<1e-3);
    int m = s1.size(), n = s2.size();
    std::vector<std::vector<int>> H(m + 1, std::vector<int>(n + 1, 0));
    std::vector<std::vector<int>> L1(m + 1, std::vector<int>(n + 1, 0));
    std::vector<std::vector<int>> H2(m + 1, std::vector<int>(n + 1, 0));
    std::vector<std::vector<int>> L2(m + 1, std::vector<int>(n + 1, 0));
    std::vector<int> max_scores(thresholds.size(), 0);
    std::vector<std::pair<int, int>> end_pos(thresholds.size(), {-1, -1});

    for (int i = 1; i <= m; ++i) {
        for (int j = 1; j <= n; ++j) {
            int score_diag = (s1[i - 1] == s2[j - 1]) ? H[i - 1][j - 1] + match : 0;

            int score_up = H[i - 1][j] + gap;

            int score_left = H[i][j - 1] + gap;

            H[i][j] = std::max({0, score_diag, score_up, score_left});

            if (H[i][j] == 0){
                H2[i][j] = 0;
                L1[i][j] = 0;
                L2[i][j] = 0;
            }
            else {
                if (H[i][j] == score_diag){
                    H2[i][j] = H2[i - 1][j - 1] + 1;
                    L1[i][j] = L1[i - 1][j - 1] + 1;
                    L2[i][j] = L2[i - 1][j - 1] + 1;
                }
                else if (H[i][j] == score_up){
                    H2[i][j] = H2[i-1][j];
                    L1[i][j] = L1[i-1][j] + 1;
                    L2[i][j] = L2[i-1][j];
                }
                else {
                    H2[i][j] = H2[i][j-1];
                    L1[i][j] = L1[i][j-1];
                    L2[i][j] = L2[i][j-1]+1;
                }
            }
            double similarity = static_cast<double>(H2[i][j]) / std::max(L1[i][j], L2[i][j]);
            if (H[i][j] == 0) {
                continue;
            }
            for (int k = 0; k < thresholds.size(); k++){
                double threshold = thresholds[k];
                if (similarity >= threshold && std::max(L1[i][j], L2[i][j]) >= max_scores[k]) {
                    max_scores[k] = std::max(L1[i][j], L2[i][j]);
                    end_pos[k] = {i, j};
                }
            }
        }
    }
    std::unordered_map<int, std::pair<int, int>> end_pos_map;
    std::vector<double> similarities(thresholds.size(), 0.0);
    for (int k = 0; k < thresholds.size(); k++){
        int matches=0;
        int alignment_length=0;
        int num_gaps1 = 0;
        int num_gaps2 = 0;

        int i = end_pos[k].first, j = end_pos[k].second;
        if (end_pos_map.find(end_pos[k].first*(n+1)+end_pos[k].second) != end_pos_map.end()){
            max_scores[k] = std::max(L1[end_pos[k].first][end_pos[k].second], L2[end_pos[k].first][end_pos[k].second]);
            similarities[k] = static_cast<double>(H2[end_pos[k].first][end_pos[k].second]) / std::max(L1[end_pos[k].first][end_pos[k].second], L2[end_pos[k].first][end_pos[k].second]);
            end_pos[k] = end_pos_map[end_pos[k].first*(n+1)+end_pos[k].second];
            continue;
        }
        while (i > 0 && j > 0 && H[i][j] > 0) {
            if (s1[i - 1] == s2[j - 1]) {
                i--; j--;
                matches++;
            } else if (H[i - 1][j] + gap == H[i][j]) {
                i--;
                num_gaps1++;
            } else {
                j--;
                num_gaps2++;
            }
        }
        alignment_length = matches + std::max(num_gaps1, num_gaps2);
        max_scores[k] = alignment_length;
        double similarity = alignment_length > 0 ? static_cast<double>(matches) / alignment_length : 0.0;
        similarities[k] = similarity;
        end_pos_map[end_pos[k].first*(n+1)+end_pos[k].second] = {i, j};
        // std::cout << "Alignment length: " << alignment_length << " Matches: " << matches << " Gaps: " << num_gaps1 << " " << num_gaps2 << " " << std::max(L1[end_pos[k].first][end_pos[k].second], L2[end_pos[k].first][end_pos[k].second]) << " " << H2[end_pos[k].first][end_pos[k].second] << "\n";
        end_pos[k] = {i, j};
    }
    std::vector<std::tuple<int, double, int, int>> res;
    for (int k = 0; k < thresholds.size(); k++){
        res.push_back({max_scores[k], similarities[k], end_pos[k].first, end_pos[k].second});
    }
    return res;
}

std::tuple<int, bool, int, int> levensthein_after_smith_waterman(const std::vector<int>& s1, const std::vector<int>& s2){
    // After calling the smith_waterman algorithm with different treshholds, I am using the approx. starting indices identified by the algorithm to find the longest length match, starting from those indices
    // These heuristics together reduce the search space enough for my algorithm to be O(n^2), as opposed to O(n^4).
    std::vector<std::tuple<int, double, int, int>> res=smith_waterman_80_similarity(s1, s2, 3, -2, {0.8, 0.9, 0.95});
    // for (auto r: res){
    //     std::cout << std::get<0>(r) << " " << std::get<1>(r) << " " << std::get<2>(r) << " " << std::get<3>(r) << "\n";
    // }
    int start_i=std::get<2>(res[0]);
    int start_j=std::get<3>(res[0]);   
    if (start_i == -1 || start_j == -1){
        return {0,0,0,0};
    }
    int m=s1.size()-start_i;
    int n=s2.size()-start_j;
    std::vector<std::vector<int>> dp(m+1, std::vector<int>(n+1, 0));
    for (int i=0; i<=m; i++){
        dp[i][0]=0;
    }
    for (int i=0; i<=n; i++){
        dp[0][i]=0;
    }
    int max_length=0;
    for (int i=1; i<=m; i++){
        for (int j=1; j<=n; j++){
            if (s1[start_i+i-1]==s2[start_j+j-1]){
                dp[i][j]=dp[i-1][j-1]+1;
            }
            else {
                dp[i][j]=std::max(dp[i-1][j], dp[i][j-1]);
            }
            if (static_cast<double>(dp[i][j])/std::max(i, j) >= 0.8 && std::max(i, j) >= max_length){
                max_length=std::max(i, j);
            }
        }
    }
    if (max_length < 30){
        // std::cout << 0 << " " << 0 << " " << 0 << "\n";
        // std::cout << 0 << " " << 0 << " " << 0 << " " << "\n";
        return {0,0,0,0};
    }
    // std::cout << max_length << " " << std::get<0>(res[1]) << " " << std::get<0>(res[2]) << "\n";
    double percentage_overlap_80=static_cast<double>(max_length)/std::max(s1.size(), s2.size());
    double percentage_overlap_90=static_cast<double>(std::get<0>(res[1]))/std::max(s1.size(), s2.size());
    double percentage_overlap_95=static_cast<double>(std::get<0>(res[2]))/std::max(s1.size(), s2.size());
    bool result=((percentage_overlap_80>0.6)&&(percentage_overlap_90>0.15));
    // std::cout << percentage_overlap_80 << " " << percentage_overlap_90 << " " << percentage_overlap_95 << " " << result << "\n";
    return {max_length, result, start_i, start_j};
}


std::array<int, 5> match_submissions(std::vector<int> &submission1, 
        std::vector<int> &submission2) {
    // std::ios_base::sync_with_stdio(false);
    // std::cin.tie(NULL);
    double start_time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    // for (int i = 0; i < submission1.size(); i++) {
    //     std::cout << submission1[i] << " ";
    // }
    // std::cout << "\n";
    // for (int i = 0; i < submission2.size(); i++) {
    //     std::cout << submission2[i] << " ";
    // }
    // std::cout << "\n";
    // std::cout << "Submission Sizes: " << submission1.size() << " " << submission2.size() << "\n";
    std::pair<int, int> res1;
    std::pair<int, int> res2;
    int longest_match=0;
    int longest_non_exact_match=0;
    {
        // Moving this to a separate scope so that immediately once the scope ends, the suffix trees can be destructed, thus saving memory.
        SuffixTree tree1(submission1);
        res1=tree1.deterministic_check(submission2, std::vector<bool>(submission2.size(), false));
        // std::cout << "Deterministic Check results: " << res1.first << " ";
        SuffixTree tree2(submission2);
        res2=tree2.deterministic_check(submission1, std::vector<bool>(submission1.size(), false));
        // std::cout << res2.first << "\n";
        longest_non_exact_match=std::min(res1.second, res2.second);
        // std::cout << "Longest non-exact match: " << std::min(res1.second, res2.second) << "\n";
        longest_match=tree1.longest_perfect_match(submission2); 
        // tree2.longest_perfect_match(submission1);
    }
    std::tuple<int, bool, int, int> res3=levensthein_after_smith_waterman(submission1, submission2);
    // std::cout << std::get<0>(res3) << " " << std::get<1>(res3) << " " << std::get<2>(res3) << " " << std::get<3>(res3) << "\n";
    std::array<int, 5> result = {(std::get<1>(res3) || (longest_match >= 80 && longest_non_exact_match >= 90)), std::min(res1.first, res2.first), std::get<0>(res3), std::get<2>(res3), std::get<3>(res3)};
    double end_time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    // std::cout << "Time taken: " << end_time-start_time << " Milliseconds\n";
    // For setting the flag, we are checking for either 60% of the length of the codes having 80% overlap and 15% of the length of the codes having 90% overlap, 
    // or exact matches of length >= 80 (along with an approx. match of length >= 90, with this "approx" match accounting for some degree of transpositions of code block).
    // This heuristic accounts for both the cases when a student copies a significant portion of another code but then modifies it (to a 20% extent), or when he copies one single function or so, and does not make many modifications.
    // std::cout << "Final result: " << result[0] << " " << result[1] << " " << result[2] << " " << result[3] << " " << result[4] << "\n";
    return result; // dummy return
    // End TODO
}

