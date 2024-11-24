#include <bits/stdc++.h>
#include "helpers.h"
#include "config.h"

using namespace std;

vector<string> LCS(vector<string> a, vector<string> b){
    int n = a.size();
    int m = b.size();
    vector<vector<int>> dp(n + 1, vector<int>(m + 1, 0));
    for(int i = 1; i <= n; i++){
        for(int j = 1; j <= m; j++){
            if(a[i - 1] == b[j - 1]){
                dp[i][j] = dp[i - 1][j - 1] + 1;
            } else{
                dp[i][j] = max(dp[i - 1][j], dp[i][j - 1]);
            }
        }
    }
    vector<string> lcs;
    int i = n, j = m;
    while(i > 0 && j > 0){
        if(a[i - 1] == b[j - 1]){
            lcs.push_back(a[i - 1]);
            i--;
            j--;
        } else if(dp[i - 1][j] > dp[i][j - 1]){
            i--;
        } else{
            j--;
        }
    }
    reverse(lcs.begin(), lcs.end());
    return lcs;
}


vector<long long> GetFingerPrints(vector<long long> hashs) {
    vector<long long> fingerPrints;
    set<pair<long long, int>> window;  // value, index
    map<pair<long long, int>, int> count;
    int i = 0;
    for (; i < WINDOW_SIZE; i++) {
        window.insert({hashs[i], i});
    }
    fingerPrints.push_back(window.begin()->first);
    count[*window.begin()]++;
    for (; i < hashs.size(); i++) {
        window.insert({hashs[i], i});
        window.erase({hashs[i - WINDOW_SIZE], i - WINDOW_SIZE});
        count[*window.begin()]++;
        if (count[*window.begin()] == 1) {
            fingerPrints.push_back(window.begin()->first);
        }
    }
    return fingerPrints;
}

vector<long long> Hash_n_Grams(vector<string> grams) {
    vector<long long> hashes;
    for (int i = 0; i < grams.size(); i++) {
        long long hash = 0;

        for (int j = 0; j < grams[i].size(); j++) {
            hash += (grams[i][j] - '!' + 1) * pow(PRIME, j);
        }
        hashes.push_back(hash);
    }
    return hashes;
}

vector<string> Generate_n_grams(string code) {
    vector<string> grams;
    for (int i = 0; i + GRAMS <= code.size(); i++) {
        grams.push_back(code.substr(i, GRAMS));
    }
    return grams;
}


void showUsage() {
    cout << "Usage: ./main <directory_path> [options]\n";
    cout << "Options:\n";
    cout << "  --threshold, -t <value>                           Set the threshold value for similarity (default: 40)\n";
    cout << "  --exclude-problems, -e <problem1,problem2,...>    Exclude problems from comparison\n"; 
    cout << "  --include-problems, -i <problem1,problem2,...>    Include only these problems in comparison\n";   
    cout << "  --include-users, -u <user1,user2,...>             Include only these users in comparison\n";
    cout << "  --window-size, -w <value>                         Set the window size for fingerprinting (default: 5)\n";
    cout << "  --grams, -g <value>                               Set the n-grams value for hashing (default: 3)\n";
    cout << "  --prime, -p <value>                               Set the prime value for hashing (default: 31)\n";
    cout << "  --help, -h                                        Show this help message\n";

}
