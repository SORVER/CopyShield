#pragma once

#include <vector>
#include <iostream>
#include <string>
using namespace std;


double JaccardSimilarity(vector<long long> fingerPrints1, vector<long long> fingerPrints2);
double CosinSimilarity(vector<long long> fingerPrints1, vector<long long> fingerPrints2);
void Compare(string path);
