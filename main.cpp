#include <bits/stdc++.h>
using namespace std;

double GetSimilarity(vector<long long> fingerPrints1, vector<long long> fingerPrints2){
    set<long long> intersection;
    set<long long> unionSet;
    for(long long i : fingerPrints1){
        unionSet.insert(i);
    }
    for(long long i : fingerPrints2){
        unionSet.insert(i);
    }
    for(long long i : fingerPrints1){
        if(find(fingerPrints2.begin(), fingerPrints2.end(), i) != fingerPrints2.end()){
            intersection.insert(i);
        }
    }
    return (double)intersection.size() / unionSet.size();
}

vector<long long> GetFingerPrints(vector<long long>hashs, int windowSize){
    vector<long long> fingerPrints;
    set<pair<long long , int>> window;  // value, index
    map<pair<long long, int>, int> count;
    int i = 0;
    for(; i<windowSize; i++){
        window.insert({hashs[i], i});
    }
    fingerPrints.push_back(window.begin()->first);
    count[*window.begin()]++;
    for(; i<hashs.size(); i++){
        window.insert({hashs[i], i});
        window.erase({hashs[i-windowSize], i-windowSize});
        count[*window.begin()]++;
        if(count[*window.begin()] == 1){
            fingerPrints.push_back(window.begin()->first);
        }
    }
    return fingerPrints;
}

vector<long long> Hash_n_Grams(vector<string> grams){
    vector<long long> hashes;
    for(int i=0; i<grams.size(); i++){
        long long hash = 0;
        int p = 31;

        for(int j=0; j<grams[i].size(); j++){
            hash += (grams[i][j] - '!' + 1) * pow(p, j);
        }
        hashes.push_back(hash);
    }
    // for(long long k : hashes){
    //     cout << k << '\n';
    // }
    return hashes;
}

vector<string> Generate_n_grams(string code, int n){
    vector<string> grams;
    for(int i=0; i+n<code.size(); i++){
        grams.push_back(code.substr(i, n));
    }
    // for(string i : grams){
    //     cout << i << '\n';
    // }
    return grams;
}

void TextProccessing(string& code){
    // remove all comments
    code = regex_replace(code, regex("//.*"), "");
    code = regex_replace(code, regex("/\\*.*?\\*/"), "");
    // code = regex_replace(code, regex("[^a-zA-Z0-9]"), " ");

    // remove all withespace
    code.erase(remove(code.begin(), code.end(), ' '), code.end());
    code.erase(remove(code.begin(), code.end(), '\n'), code.end());
    code.erase(remove(code.begin(), code.end(), '\t'), code.end());

    // transform the code to lowercase 
    transform(code.begin(), code.end(), code.begin(), ::tolower);
}

int main(){
    string code1 = "#include <iostream>\n#include <vector>\n#include <string>\n#include <algorithm>\n#include <regex>\n#include <cmath>\n\nusing namespace std;\n\n// Function to calculate hashes for n-grams\nvector<long long> ComputeNGramHashes(const vector<string>& nGrams) {\n    vector<long long> hashValues;\n    const int hashBase = 31;\n\n    for (const auto& nGram : nGrams) {\n        long long currentHash = 0;\n\n        for (int index = 0; index < nGram.size(); index++) {\n            currentHash += (nGram[index] - '!' + 1) * pow(hashBase, index);\n        }\n        hashValues.push_back(currentHash);\n    }\n    return hashValues;\n}\n\n// Function to generate n-grams from a given string\nvector<string> GenerateNGrams(const string& sourceCode, int n) {\n    vector<string> nGramsList;\n    for (int position = 0; position + n <= sourceCode.size(); position++) {\n        nGramsList.push_back(sourceCode.substr(position, n));\n    }\n    return nGramsList;\n}\n\n// Function to process the code by removing comments and unnecessary whitespace\nvoid ProcessText(string& sourceCode) {\n    // Remove single-line comments\n    sourceCode = regex_replace(sourceCode, regex(\"//.*\"), \"\");\n    // Remove multi-line comments\n    sourceCode = regex_replace(sourceCode, regex(\"/\\*.*?\\*/\"), \"\");\n    \n    // Remove all whitespace characters\n    sourceCode.erase(remove(sourceCode.begin(), sourceCode.end(), ' '), sourceCode.end());\n    sourceCode.erase(remove(sourceCode.begin(), sourceCode.end(), '\\n'), sourceCode.end());\n    sourceCode.erase(remove(sourceCode.begin(), sourceCode.end(), '\\t'), sourceCode.end());\n\n    // Convert the code to lowercase\n    transform(sourceCode.begin(), sourceCode.end(), sourceCode.begin(), ::tolower);\n}";
    string code2 = "vector<long long> HashNGrams(vector<string> nGrams) {\n    vector<long long> hashValues;\n    for(int index = 0; index < nGrams.size(); index++) {\n        long long currentHash = 0;\n        int primeBase = 31;\n\n        for(int position = 0; position < nGrams[index].size(); position++) {\n            currentHash += (nGrams[index][position] - '!' + 1) * pow(primeBase, position);\n        }\n        hashValues.push_back(currentHash);\n    }\n    // for(long long value : hashValues) {\n    //     cout << value << '\\n';\n    // }\n    return hashValues;\n}\n\nvector<string> GenerateNGrams(string sourceCode, int n) {\n    vector<string> nGramsList;\n    for(int index = 0; index + n < sourceCode.size(); index++) {\n        nGramsList.push_back(sourceCode.substr(index, n));\n    }\n    // for(string nGram : nGramsList) {\n    //     cout << nGram << '\\n';\n    // }\n    return nGramsList;\n}\n\nvoid ProcessText(string& sourceCode) {\n    // Remove all comments\n    sourceCode = regex_replace(sourceCode, regex(\"//.*\"), \"\");\n    sourceCode = regex_replace(sourceCode, regex(\"/\\*.*?\\*/\"), \"\");\n    // sourceCode = regex_replace(sourceCode, regex(\"[^a-zA-Z0-9]\"), \" \");\n\n    // Remove all whitespace\n    sourceCode.erase(remove(sourceCode.begin(), sourceCode.end(), ' '), sourceCode.end());\n    sourceCode.erase(remove(sourceCode.begin(), sourceCode.end(), '\\n'), sourceCode.end());\n    sourceCode.erase(remove(sourceCode.begin(), sourceCode.end(), '\\t'), sourceCode.end());\n\n    // Transform the code to lowercase\n    transform(sourceCode.begin(), sourceCode.end(), sourceCode.begin(), ::tolower);\n}";

    
    TextProccessing(code1);
    TextProccessing(code2);

    vector<long long> hashes1 = Hash_n_Grams(Generate_n_grams(code1, 5));
    vector<long long> fingerPrints1 = GetFingerPrints(hashes1, 5);

    vector<long long> hashes2 = Hash_n_Grams(Generate_n_grams(code2, 5));
    vector<long long> fingerPrints2 = GetFingerPrints(hashes2, 5);


    double similarity = GetSimilarity(fingerPrints1, fingerPrints2);
    cout << similarity << endl;
    
    // cout << code << endl;
    return 0;
}