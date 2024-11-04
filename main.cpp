#include <bits/stdc++.h>
#include <dirent.h>
#include <fstream>
using namespace std;

vector<shared_ptr<ifstream>> files;

double GetSimilarity(vector<long long> fingerPrints1, vector<long long> fingerPrints2) {  // Jaccard Similarity
    set<long long> intersection;
    set<long long> unionSet;
    for (long long i : fingerPrints1) {
        unionSet.insert(i);
    }
    for (long long i : fingerPrints2) {
        unionSet.insert(i);
    }
    for (long long i : fingerPrints1) {
        if (find(fingerPrints2.begin(), fingerPrints2.end(), i) != fingerPrints2.end()) {
            intersection.insert(i);
        }
    }
    return (double)intersection.size() / unionSet.size();
}

vector<long long> GetFingerPrints(vector<long long> hashs, int windowSize) {
    vector<long long> fingerPrints;
    set<pair<long long, int>> window;  // value, index
    map<pair<long long, int>, int> count;
    int i = 0;
    for (; i < windowSize; i++) {
        window.insert({hashs[i], i});
    }
    fingerPrints.push_back(window.begin()->first);
    count[*window.begin()]++;
    for (; i < hashs.size(); i++) {
        window.insert({hashs[i], i});
        window.erase({hashs[i - windowSize], i - windowSize});
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
        int p = 31;

        for (int j = 0; j < grams[i].size(); j++) {
            hash += (grams[i][j] - '!' + 1) * pow(p, j);
        }
        hashes.push_back(hash);
    }
    return hashes;
}

vector<string> Generate_n_grams(string code, int n) {
    vector<string> grams;
    for (int i = 0; i + n <= code.size(); i++) {
        grams.push_back(code.substr(i, n));
    }
    return grams;
}

void TextProcessing(string& code) {
    // remove all comments
    code = regex_replace(code, regex("//.*"), "");
    code = regex_replace(code, regex("/\\*.*?\\*/"), "");

    // remove all whitespace
    code.erase(remove(code.begin(), code.end(), ' '), code.end());
    code.erase(remove(code.begin(), code.end(), '\n'), code.end());
    code.erase(remove(code.begin(), code.end(), '\t'), code.end());

    // transform the code to lowercase
    transform(code.begin(), code.end(), code.begin(), ::tolower);
}

void Compare() {
    for (int i = 0; i < files.size(); i++) {
        for (int j = i + 1; j < files.size(); j++) {
            string code1, code2;
            getline(*files[i], code1, '\0');
            getline(*files[j], code2, '\0');
            TextProcessing(code1);
            TextProcessing(code2);
            // cout << code1 << ' ' << code2 << '\n';
            vector<string> grams1 = Generate_n_grams(code1, 3);
            vector<string> grams2 = Generate_n_grams(code2, 3);
            vector<long long> hashes1 = Hash_n_Grams(grams1);
            vector<long long> hashes2 = Hash_n_Grams(grams2);
            vector<long long> fingerPrints1 = GetFingerPrints(hashes1, 5);
            vector<long long> fingerPrints2 = GetFingerPrints(hashes2, 5);
            double similarity = GetSimilarity(fingerPrints1, fingerPrints2);
            similarity *= 100;
            if(similarity > 40){
                cout << "File " << i << " and file " << j << " are likely most the same\n";
            }
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        cout << "Please provide the dir path\n";
        return 0;
    }
    string dirPath = argv[1];
    DIR* dir = opendir(dirPath.c_str());
    try {
        if (dir == NULL) {
            cerr << "Failed to open the Directory\n";
            return 0;
        }
        struct dirent* entry;
        while ((entry = readdir(dir)) != NULL) {
            if (entry->d_type == DT_REG) {
                string filePath = dirPath + "/" + entry->d_name;
                shared_ptr<ifstream> file = make_shared<ifstream>(filePath);
                if (!file->is_open()) {
                    cerr << "Failed to open the file " << filePath << '\n';
                    continue;
                }
                files.push_back(file);
            }
        }
    } catch (const exception& e) {
        cout << "Error: " << e.what() << '\n';
    }
    closedir(dir);
    Compare();
    return 0;
}
