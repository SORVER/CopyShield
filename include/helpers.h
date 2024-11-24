# pragma once 

#include <string>
#include <vector>

using namespace std;

vector<long long> GetFingerPrints(vector<long long> hashs);
vector<long long> Hash_n_Grams(vector<string> grams);
vector<string> Generate_n_grams(string code);
void showUsage();
vector<string> LCS(vector<string> a, vector<string> b);