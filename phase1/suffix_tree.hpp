#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>
#include <unordered_set>
#include <set>
#include<array>

using namespace std;

vector<int> buildSuffixArray(const vector<int>& s) {
    int n = s.size();
    vector<int> sa(n), rank(n), tempRank(n), cnt(max(256, n));
    
    iota(sa.begin(), sa.end(), 0);
    for (int i = 0; i < n; ++i) rank[i] = s[i];

    for (int len = 1; len < n; len <<= 1) {
        auto cmp = [&](int a, int b) {
            if (rank[a] != rank[b]) return rank[a] < rank[b];
            int ra = (a + len < n) ? rank[a + len] : -1;
            int rb = (b + len < n) ? rank[b + len] : -1;
            return ra < rb;
        };
        sort(sa.begin(), sa.end(), cmp);
        
        tempRank[sa[0]] = 0;
        for (int i = 1; i < n; ++i) {
            tempRank[sa[i]] = tempRank[sa[i - 1]] + cmp(sa[i - 1], sa[i]);
        }
        rank = tempRank;
    }
    return sa;
}

vector<int> buildLCPArray( vector<int>& s,  vector<int>& sa) {
    int n = s.size();
    vector<int> lcp(n), rank(n);
    for (int i = 0; i < n; ++i) rank[sa[i]] = i;

    int h = 0;
    for (int i = 0; i < n; ++i) {
        if (rank[i] > 0) {
            int j = sa[rank[i] - 1];
            while (i + h < n && j + h < n && s[i + h] == s[j + h]) h++;
            lcp[rank[i]] = h;
            if (h > 0) h--;
        }
    }
    return lcp;
}

std::array<int, 5> match_submissions( vector<int>& arr1,  vector<int>& arr2) {
    int n = arr1.size(), m = arr2.size();
    vector<int> concatArray(arr1.begin(), arr1.end());
    concatArray.push_back(-1); // Separator
    concatArray.insert(concatArray.end(), arr2.begin(), arr2.end());
    
    vector<int> sa = buildSuffixArray(concatArray);
    vector<int> lcp = buildLCPArray(concatArray, sa);

    int totalLength = 0;
    set<pair<int, int>> countedPatterns; // To track counted indices from arr1 and arr2

    for (int i = 1; i < sa.size(); ++i) {
        // Check if one suffix is in arr1 part and the other in arr2 part
        if ((sa[i] < n && sa[i - 1] > n) || (sa[i] > n && sa[i - 1] < n)) {
            int commonLength = lcp[i];
            if (commonLength >= 10) {
                int validLength = min(commonLength, 20); // Count only up to 20

                // Identify starting positions in arr1 and arr2
                int start1 = min(sa[i], sa[i - 1]); // Start index in arr1
                int start2 = max(sa[i], sa[i - 1]) - (n + 1); // Start index in arr2

                // Only count if this pair hasn't been counted before
                if (countedPatterns.find({start1, start2}) == countedPatterns.end()) {
                    countedPatterns.insert({start1, start2});
                    totalLength += validLength;
                }
            }
        }
    }
    
    std::array<int, 5> result;
        result[1] = totalLength; // Example assignment, adjust as needed
        return result;
}


