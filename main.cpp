#include <bits/stdc++.h>
using namespace std;

vector<long long> Hash_n_Grams(vector<string> grams){
    vector<long long> hashes;
    for(int i=0; i<grams.size(); i++){
        long long hash = 0;
        int p = 31;

        for(int j=0; j<grams[i].size(); j++){
            hash += (grams[i][j] - 'a' + 1) * pow(p, j);
        }
        hashes.push_back(hash);
    }
    for(long long k : hashes){
        cout << k << '\n';
    }
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
    code = regex_replace(code, regex("[^a-zA-Z0-9]"), " ");

    // remove all withespace
    code.erase(remove(code.begin(), code.end(), ' '), code.end());
    code.erase(remove(code.begin(), code.end(), '\n'), code.end());
    code.erase(remove(code.begin(), code.end(), '\t'), code.end());

    // transform the code to lowercase 
    transform(code.begin(), code.end(), code.begin(), ::tolower);
}

int main(){
    string code = "int main(){\n\t// this is a comment\n\t/* this is a comment */\n\tcout << \"asdasdadsadad\";\nreturn 0;\n}";
    TextProccessing(code);
    Hash_n_Grams(Generate_n_grams(code, 5));
    // cout << code << endl;
    return 0;
}